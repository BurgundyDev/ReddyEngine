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
            changeSelection({pEntity});
        for (const auto& pChild : children)
            drawEntitySceneTree(pChild);
        ImGui::TreePop();
    }
    else
    {
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            changeSelection({pEntity});
    }
}

void EditorState::onMouseDown(Engine::IEvent* pEvent)
{
    if (ImGui::GetIO().WantCaptureMouse || m_editDocumentType != EditDocumentType::Scene) return;

    auto pDownEvent = (Engine::MouseButtonDownEvent*)pEvent;
    if (pDownEvent->button.button != SDL_BUTTON_LEFT) return;

    m_isMouseDownInWorld = true;

    auto ctrl = Engine::getInput()->isKeyDown(SDL_SCANCODE_LCTRL);
    auto shift = Engine::getInput()->isKeyDown(SDL_SCANCODE_LSHIFT);
    auto alt = Engine::getInput()->isKeyDown(SDL_SCANCODE_LALT);

    const auto& pHoveredEntity = Engine::getScene()->getHoveredEntity();

    if (!pHoveredEntity && !ctrl && !m_selected.empty())
        changeSelection({}); // Deselect

    if (pHoveredEntity)
    {
        if (!pHoveredEntity->isSelected)
        {
            if (!ctrl)
            {
                changeSelection({pHoveredEntity});
            }
            else
            {
                auto newSelection = m_selected;
                newSelection.push_back(pHoveredEntity);
                changeSelection(newSelection);
            }
        }

        m_isMouseDownInWorld = true;
        m_mouseOnDown = m_mouseWorldPos;
    }
}

void EditorState::onMouseUp(Engine::IEvent* pEvent)
{
    if (!m_isMouseDownInWorld) return;

    auto pDownEvent = (Engine::MouseButtonDownEvent*)pEvent;
    if (pDownEvent->button.button != SDL_BUTTON_LEFT) return;

    m_isMouseDownInWorld = false;

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
                if (ctrl)
                {
                    auto newSelected = m_selected;
                    for (auto it = newSelected.begin(); it != newSelected.end(); ++it)
                    {
                        if (*it == pHoveredEntity)
                        {
                            newSelected.erase(it);
                        }
                    }
                    changeSelection(newSelected);
                }
                else
                    changeSelection({pHoveredEntity});
            }
            break;
        }
        case TransformType::Translate:
        {
            auto entities = m_selected;
            std::vector<Json::Value> jsonsBefore;
            std::vector<Json::Value> jsonsAfter;
            for (const auto& pEntity : entities)
            {
                jsonsBefore.push_back(pEntity->undoJson);
                pEntity->undoJson = pEntity->serialize(false);
                jsonsAfter.push_back(pEntity->undoJson);
            }

            m_pActionManager->addAction("Move", [this, entities, jsonsAfter]()
            {
                for (int i = 0, len = (int)entities.size(); i < len; ++i)
                {
                    const auto& pEntity = entities[i];
                    pEntity->deserialize(jsonsAfter[i], false);
                }
                setDirty(true);
            },[this, entities, jsonsBefore]
            {
                for (int i = 0, len = (int)entities.size(); i < len; ++i)
                {
                    const auto& pEntity = entities[i];
                    pEntity->deserialize(jsonsBefore[i], false);
                }
                setDirty(true);
            });
            break;
        }
    }

    m_transformType = TransformType::None;
}

void EditorState::updateTransform()
{
    if (!m_isMouseDownInWorld) return;

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
                auto prevJson = pEntity->undoJson;
                auto newJson = pEntity->serialize();
                setDirty(true);
                m_pActionManager->addAction("Edit Entity", [this, pEntity, newJson]()
                {
                    pEntity->deserialize(newJson);
                    setDirty(true);
                }, [this, pEntity, prevJson]()
                {
                    pEntity->deserialize(prevJson);
                    setDirty(true);
                });
            }
        }
    }
    Engine::GUI::endEditorWindow();
}
