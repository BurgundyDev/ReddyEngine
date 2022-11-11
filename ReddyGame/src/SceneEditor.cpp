#include "EditorState.h"
#include "ActionManager.h"

#include <Engine/Entity.h>
#include <Engine/Scene.h>
#include <Engine/ReddyEngine.h>
#include <Engine/SpriteComponent.h>
#include <Engine/GUI.h>
#include <Engine/Input.h>

#include <imgui.h>


const char* EditorState::getEntityFriendlyName(const Engine::EntityRef& pEntity)
{
    if (!pEntity->name.empty()) return pEntity->name.c_str();
    if (pEntity->hasComponent<Engine::SpriteComponent>()) return "Sprite";
    return "Entity";
}

void EditorState::drawEntitySceneTree(const Engine::EntityRef& pEntity)
{
    const auto& children = pEntity->getChildren();

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
    if (children.empty()) flags |= ImGuiTreeNodeFlags_Leaf;
    if (pEntity->isSelected) flags |= ImGuiTreeNodeFlags_Selected;

    if (ImGui::TreeNodeEx(pEntity.get(), flags, getEntityFriendlyName(pEntity)))
    {
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            changeSelectionAction({pEntity});
        for (const auto& pChild : children)
            drawEntitySceneTree(pChild);
        ImGui::TreePop();
    }
    else
    {
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            changeSelectionAction({pEntity});
    }
}

void EditorState::onMouseDown(Engine::IEvent* pEvent)
{
    if (ImGui::GetIO().WantCaptureMouse || m_editDocumentType != EditDocumentType::Scene) return;

    auto pDownEvent = (Engine::MouseButtonDownEvent*)pEvent;
    m_mouseButton = pDownEvent->button.button;
    m_mouseScreenOnDown = Engine::getInput()->getMousePos();
    m_isMouseDownInWorld = true;
    m_wasAddedToSelection = false;

    if (m_mouseButton == SDL_BUTTON_LEFT)
    {
        auto ctrl = Engine::getInput()->isKeyDown(SDL_SCANCODE_LCTRL);
        auto shift = Engine::getInput()->isKeyDown(SDL_SCANCODE_LSHIFT);
        auto alt = Engine::getInput()->isKeyDown(SDL_SCANCODE_LALT);

        const auto& pHoveredEntity = Engine::getScene()->getHoveredEntity();

        if (!pHoveredEntity && !ctrl && !shift && !m_selected.empty())
            changeSelectionAction({}); // Deselect

        if (pHoveredEntity)
        {
            if (!pHoveredEntity->isSelected)
            {
                if (!ctrl && !shift)
                {
                    changeSelectionAction({pHoveredEntity});
                }
                else
                {
                    auto newSelection = m_selected;
                    newSelection.push_back(pHoveredEntity);
                    changeSelectionAction(newSelection);
                    m_wasAddedToSelection = true;
                }
            }

            m_isMouseDownInWorld = true;
            m_mouseOnDown = m_mouseWorldPos;
        }
    }
}

void EditorState::onMouseUp(Engine::IEvent* pEvent)
{
    if (!m_isMouseDownInWorld) return;
    m_isMouseDownInWorld = false;

    auto pDownEvent = (Engine::MouseButtonDownEvent*)pEvent;
    if (pDownEvent->button.button == SDL_BUTTON_RIGHT)
    {
        if (m_mouseScreenOnDown == Engine::getInput()->getMousePos())
            m_openCreateEntityMenu = true;
        return;
    }
    if (pDownEvent->button.button != SDL_BUTTON_LEFT) return;


    auto ctrl = Engine::getInput()->isKeyDown(SDL_SCANCODE_LCTRL);
    auto shift = Engine::getInput()->isKeyDown(SDL_SCANCODE_LSHIFT);
    auto alt = Engine::getInput()->isKeyDown(SDL_SCANCODE_LALT);

    const auto& pHoveredEntity = Engine::getScene()->getHoveredEntity();
    
    // Finish transform
    switch (m_transformType)
    {
        case TransformType::None:
        {
            if (pHoveredEntity && pHoveredEntity->isSelected) // We clicked an item without draging the mouse, we select/deselect it
            {
                if (ctrl || shift)
                {
                    if (!m_wasAddedToSelection)
                    {
                        auto newSelected = m_selected;
                        for (auto it = newSelected.begin(); it != newSelected.end(); ++it)
                        {
                            if (*it == pHoveredEntity)
                            {
                                newSelected.erase(it);
                                break;
                            }
                        }
                        changeSelectionAction(newSelected);
                    }
                }
                else
                    changeSelectionAction({pHoveredEntity});
            }
            break;
        }
        case TransformType::Translate:
        {
            pushUndo("Move");
            break;
        }
    }

    m_transformType = TransformType::None;
}

void EditorState::updateTransform()
{
    if (!m_isMouseDownInWorld) return;
    if (m_mouseButton != SDL_BUTTON_LEFT) return; // So much hacks

    switch (m_transformType)
    {
        case TransformType::None:
        {
            auto diff = m_mouseWorldPos - m_mouseOnDown;
            if (glm::length(diff) > 3.0f / m_zoomf)
            {
                // We started moving!
                m_transformType = TransformType::Translate;

                m_worldPositionsOnDown.clear();
                for (const auto& pEntity : m_selected)
                    m_worldPositionsOnDown.push_back(pEntity->getWorldPosition());
            }
            break;
        }
        case TransformType::Translate:
        {
            auto diff = m_mouseWorldPos - m_mouseOnDown;
            for (int i = 0, len = (int)m_selected.size(); i < len; ++i)
            {
                const auto& pEntity = m_selected[i];
                const auto& posOnDown = m_worldPositionsOnDown[i];

                pEntity->setWorldPosition(posOnDown + diff);
            }
            break;
        }
    }
}

void EditorState::drawSceneUI() // This is also kind of update
{
    // Scene tree
    if (Engine::GUI::beginEditorWindow("Scene"))
    {
        //const auto& children = Engine::getEntityManager()->getRoot()->getChildren();
        //for (const auto& pChild : children)
        //    drawEntitySceneTree(pChild);
        drawEntitySceneTree(Engine::getScene()->getRoot());

        if (Engine::GUI::statusBarButton("+", "Create Entity"))
            m_openCreateEntityMenu = true;
    }
    Engine::GUI::endEditorWindow();


    // Inspector (For selected entity/entities)
    if (Engine::GUI::beginEditorWindow("Entity Inspector"))
    {
        // For now, we only do 1 entity at a time
        if (m_selected.size() == 1)
        {
            auto pEntity = m_selected.front();
            if (pEntity->edit())
            {
                pushUndo("Edit Entity");
            }
        }
    }
    Engine::GUI::endEditorWindow();

    // Context menu
    if (m_openCreateEntityMenu)
    {
        ImGui::OpenPopup("CreateEntityContext");
        m_openCreateEntityMenu = false;
    }
    if (ImGui::BeginPopupContextWindow("CreateEntityContext"))
    {
        if (ImGui::Selectable("Empty")) onCreateEmptyEntity();
        if (ImGui::Selectable("Sprite")) onCreateSpriteEntity();
        if (ImGui::Selectable("Text")) onCreateTextEntity();
        if (ImGui::Selectable("Sound")) onCreateSoundEntity();
        if (ImGui::Selectable("Particle")) onCreateParticleEntity();
        ImGui::EndPopup();
    }
}
