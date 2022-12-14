#include "EditorState.h"
#include "ActionManager.h"

#include <Engine/Entity.h>
#include <Engine/Scene.h>
#include <Engine/Component.h>
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
static Engine::EntityRef g_pDragAfter = nullptr;
static Engine::EntityRef g_pDragBefore = nullptr;


// This is very expensive and done for every entity in the scene tree window
std::string EditorState::getEntityFriendlyName(const Engine::EntityRef& pEntity)
{
    std::string ret;
    if (pEntity->editorLocked) ret = "(LOCKED) ";
    if (!pEntity->editorVisible) ret += "(HIDDEN) ";
    if (!pEntity->name.empty()) ret += pEntity->name.c_str();
    else
    {
        const auto& components = pEntity->getComponents();
        if (components.empty()) ret += "Entity";
        else
        {
            bool found = false;
            for (const auto& pComponent : components)
            {
                auto n = pComponent->getFriendlyName();
                if (!n.empty())
                {
                    found = true;
                    ret += n;
                    break;
                }
            }
            if (!found) ret += components.front()->getType();
        }
    }
    return ret;
}

void EditorState::drawEntitySceneTree(const Engine::EntityRef& pEntity)
{
    const auto& children = pEntity->getChildren();

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    if (children.empty()) flags |= ImGuiTreeNodeFlags_Leaf;
    if (pEntity->isSelected) flags |= ImGuiTreeNodeFlags_Selected;
    //if (pEntity->expanded) flags |= ImGuiTreeNodeFlags_DefaultOpen; // No need, we do ImGui::SetNextItemOpen

    ImGui::SetNextItemOpen(pEntity->expanded);
    bool openned = pEntity->expanded = ImGui::TreeNodeEx((const void*)(uintptr_t)pEntity->id, flags, getEntityFriendlyName(pEntity).c_str());

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
        ImGui::EndDragDropTarget();
    }

    if (ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayload("_SCENETREE", NULL, 0);
        ImGui::Text("%i Entities", m_selected.size());
        ImGui::EndDragDropSource();
    }

    if (openned)
    {
        bool first = true;
        for (const auto& pChild : children)
        {
            if (first)
            {
                first = false;

                // Draw n drop items before this child
                bool dummy = false;
                auto c = ImGui::GetCursorPosY();
                ImGui::SetCursorPosY(c - 1);
                ImGui::Selectable("---", &dummy, ImGuiSelectableFlags_Disabled, ImVec2(0, 2));
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_SCENETREE"))
                    {
                        g_pDragBefore = pChild;
                    }
                    ImGui::EndDragDropTarget();
                }
                ImGui::SetCursorPosY(c);
            }
            drawEntitySceneTree(pChild);
        }
        ImGui::TreePop();
    }

    // Draw n drop items after this
    bool dummy = false;
    auto c = ImGui::GetCursorPosY();
    ImGui::SetCursorPosY(c - 1);
    ImGui::Selectable("---", &dummy, ImGuiSelectableFlags_Disabled, ImVec2(0, 2));
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_SCENETREE"))
        {
            g_pDragAfter = pEntity;
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::SetCursorPosY(c);
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
        {
            changeSelectionAction({}); // Deselect
        }

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
        else
        {
            m_boxSelect = true;
            m_boxSelectFrom = m_mouseWorldPos;
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

    if (m_boxSelect)
    {
        m_boxSelect = false;
        auto boxSelectTo = m_mouseWorldPos;

        glm::vec4 selectRect(
            glm::min(boxSelectTo.x, m_boxSelectFrom.x),
            glm::min(boxSelectTo.y, m_boxSelectFrom.y),
            glm::abs(boxSelectTo.x - m_boxSelectFrom.x),
            glm::abs(boxSelectTo.y - m_boxSelectFrom.y)
        );

        std::vector<Engine::EntityRef> entities;
        Engine::getScene()->getRoot()->getEntitiesInRect(entities, selectRect);
        if (!entities.empty())
            changeSelectionAction(entities);
        m_transformType = TransformType::None;
        return;
    }

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
            if (Engine::getInput()->isKeyDown(SDL_SCANCODE_LSHIFT))
            {
                if (std::fabsf(diff.x) > std::fabsf(diff.y)) diff.y = 0;
                else diff.x = 0;
            }
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
        g_pDragAfter = nullptr;
        g_pDragBefore = nullptr;
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

            if (valid)
            {
                for (const auto& pDragEntity : m_selected)
                {
                    g_pDragTarget->addChild(pDragEntity);
                }
                pushUndo("Scene Tree Change");
            }
        }
        else if (g_pDragAfter)
        {
            bool valid = true;
            auto pParent = g_pDragAfter->getParent();
            if (pParent)
            {
                for (const auto& pDragEntity : m_selected)
                {
                    // Make sure we're not dragging a parent into one of it's child
                    if (pDragEntity == pParent || pDragEntity->hasChild(pParent, true))
                    {
                        valid = false;
                        break;
                    }
                }

                if (valid)
                {
                    // Find index
                    int index = -1;
                    for (const auto& pDragEntity : m_selected)
                    {
                        pParent->addChild(pDragEntity, pParent->getChildIndex(g_pDragAfter) + 1);
                    }
                    pushUndo("Scene Tree Change");
                }
            }
        }
        else if (g_pDragBefore)
        {
            bool valid = true;
            auto pParent = g_pDragBefore->getParent();
            if (pParent)
            {
                for (const auto& pDragEntity : m_selected)
                {
                    // Make sure we're not dragging a parent into one of it's child
                    if (pDragEntity == pParent || pDragEntity->hasChild(pParent, true))
                    {
                        valid = false;
                        break;
                    }
                }

                if (valid)
                {
                    // Find index
                    int index = -1;
                    for (const auto& pDragEntity : m_selected)
                    {
                        pParent->addChild(pDragEntity, pParent->getChildIndex(g_pDragBefore));
                    }
                    pushUndo("Scene Tree Change");
                }
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
        ImGui::Separator();
        const auto& componentNames = Engine::ComponentFactory::getComponentNames();
        for (const auto& componentName : componentNames)
            if (ImGui::MenuItem(componentName.c_str()))
                onCreateEntity(componentName);
        ImGui::EndPopup();
    }

    if (showCreateComponentPopup)
    {
        ImGui::OpenPopup("CreateComponentContext");
        m_openCreateEntityMenu = false;
    }
    if (ImGui::BeginPopupContextWindow("CreateComponentContext"))
    {
        const auto& componentNames = Engine::ComponentFactory::getComponentNames();
        for (const auto& componentName : componentNames)
        {
            if (ImGui::Selectable(componentName.c_str()))
            {
                if (m_selected.size() != 1) return;
                m_selected.front()->addComponent(Engine::ComponentFactory::create(componentName));
                pushUndo("Add Component");
            }
        }
        ImGui::EndPopup();
    }
}
