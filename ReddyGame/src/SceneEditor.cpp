#include "EditorState.h"
#include "ActionManager.h"

#include <Engine/Entity.h>
#include <Engine/Scene.h>
#include <Engine/ReddyEngine.h>
#include <Engine/GUI.h>
#include <Engine/Input.h>
#include <Engine/ResourceManager.h>
#include <Engine/Texture.h>
#include <Engine/SpriteComponent.h>
#include <Engine/TextComponent.h>
#include <Engine/ScriptComponent.h>
#include <Engine/PFXComponent.h>

#include <filesystem>

#include <cmath>

#include <imgui.h>

static Engine::EntityRef g_pDragTarget = nullptr;


// This is very expensive and done for every entity inthe scene tree window
const char* EditorState::getEntityFriendlyName(const Engine::EntityRef& pEntity)
{
    if (!pEntity->name.empty()) return pEntity->name.c_str();
    if (pEntity->hasComponent<Engine::SpriteComponent>()) return "Sprite";
    if (pEntity->hasComponent<Engine::TextComponent>()) return pEntity->getComponent<Engine::TextComponent>()->text.c_str();
    if (pEntity->hasComponent<Engine::ScriptComponent>()) return pEntity->getComponent<Engine::ScriptComponent>()->name.c_str();
    if (pEntity->hasComponent<Engine::PFXComponent>()) return "PFX";
    return "Entity";
}

void EditorState::drawEntitySceneTree(const Engine::EntityRef& pEntity)
{
    const auto& children = pEntity->getChildren();

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
    if (children.empty()) flags |= ImGuiTreeNodeFlags_Leaf;
    if (pEntity->isSelected) flags |= ImGuiTreeNodeFlags_Selected;
    if (pEntity->expanded) flags |= ImGuiTreeNodeFlags_DefaultOpen;

    pEntity->expanded = ImGui::TreeNodeEx((const void*)(uintptr_t)pEntity->id, flags, getEntityFriendlyName(pEntity));

    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
    {
        //TODO: We should be able to select range with Shift
        if (Engine::getInput()->isKeyDown(SDL_SCANCODE_LCTRL))
        {
            auto newSelection = m_selected;
            bool wasRemoved = false;
            for (auto it = newSelection.begin(); it != newSelection.end(); ++it)
            {
                if (*it == pEntity)
                {
                    newSelection.erase(it);
                    wasRemoved = true;
                    break;
                }
            }
            if (!wasRemoved)
                newSelection.push_back(pEntity);
            changeSelectionAction(newSelection);
        }
        else
        {
            changeSelectionAction({pEntity});
        }
    }

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_SCENETREE"))
        {
            g_pDragTarget = pEntity;
        }
    }

    if (ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayload("_SCENETREE", NULL, 0);
        ImGui::Text("%i Entities", m_selected.size());
        ImGui::EndDragDropSource();
    }

    if (pEntity->expanded)
    {
        for (const auto& pChild : children)
            drawEntitySceneTree(pChild);
        ImGui::TreePop();
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
        auto ctrl = false;//Engine::getInput()->isKeyDown(SDL_SCANCODE_LCTRL);
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


    auto ctrl = false;//Engine::getInput()->isKeyDown(SDL_SCANCODE_LCTRL);
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


void EditorState::onDropEvent(Engine::IEvent* pEvent)
{
    auto pDropEvent = (Engine::DropEvent*)pEvent;
    if (!pDropEvent) return;

    const char* file = pDropEvent->drop.file;
    if (!file) return;

    const std::filesystem::path filePath(file);

    std::string extension = filePath.extension().string();

    // remove '.' from extension
    if (!extension.empty() && extension.front() == '.') {
        extension.erase(extension.begin());
    }

    bool isTextureExtension = false;

    for (const char* ext : Engine::Texture::SUPPORTED_FORMATS) {
        if (std::strcmp(ext, extension.c_str()) == 0) {
            isTextureExtension = true;
            break;
        }
    }

    if (!isTextureExtension) {
        fprintf(stderr, "Failed to load dropped file: %s\n\tFile is not a texture.\n", file);

        return;
    }
    
    std::string resultPath;

    if (!Engine::getResourceManager()->copyFileToAssets(file, "textures", resultPath)) {
        fprintf(stderr, "Failed to copy asset from %s to assets!\n", file);

        return;
    }

    Engine::TextureRef texture = Engine::getResourceManager()->getTexture(resultPath);
    if (!texture) return;

    auto pEntity = Engine::getScene()->createEntity();

    auto spriteComponent = pEntity->addComponent<Engine::SpriteComponent>();
    spriteComponent->pTexture = std::move(texture);

    createEntityAction(pEntity);
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

                const float snapScale = std::max(m_snapScale, 0.001f);

                if (m_ctrlHeld) {
                    pEntity->setWorldPosition(snapScale * glm::round((posOnDown + diff) * (1.0f / snapScale)));
                } else {
                    pEntity->setWorldPosition(posOnDown + diff);
                }
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
        g_pDragTarget = nullptr;
        drawEntitySceneTree(Engine::getScene()->getRoot());

        if (g_pDragTarget)
        {
            bool valid = true;
            for (const auto& pDragEntity : m_selected)
            {
                // Make sure we're not dragging a parent into one of it's child
                if (pDragEntity == g_pDragTarget || pDragEntity->hasChild(g_pDragTarget, true))
                {
                    valid = false;
                    break;
                }
            }

            ImGui::EndDragDropTarget();

            if (valid)
            {
                for (const auto& pDragEntity : m_selected)
                {
                    g_pDragTarget->addChild(pDragEntity);
                }
                pushUndo("Scene Tree Change");
            }
        }

        if (Engine::GUI::statusBarButton("+", "Create Entity"))
            m_openCreateEntityMenu = true;
    }
    Engine::GUI::endEditorWindow();

    // Inspector (For selected entity/entities)
    bool showCreateComponentPopup = false;
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

        // Add component
        if (Engine::GUI::statusBarButton("+", "Add Component"))
            showCreateComponentPopup = true;
    }
    Engine::GUI::endEditorWindow();

    // Context menus
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
        if (ImGui::Selectable("Script")) onCreateScriptEntity();
        ImGui::EndPopup();
    }

    if (showCreateComponentPopup)
    {
        ImGui::OpenPopup("CreateComponentContext");
        m_openCreateEntityMenu = false;
    }
    if (ImGui::BeginPopupContextWindow("CreateComponentContext"))
    {
        if (ImGui::Selectable("Sprite")) onAddComponent<Engine::SpriteComponent>();
        if (ImGui::Selectable("Text")) onAddComponent<Engine::TextComponent>();
        if (ImGui::Selectable("Script")) onAddComponent<Engine::ScriptComponent>();
        if (ImGui::Selectable("PFX")) onAddComponent<Engine::PFXComponent>();
        ImGui::EndPopup();
    }
}
