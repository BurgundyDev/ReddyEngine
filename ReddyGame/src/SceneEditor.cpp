#include "EditorState.h"

#include <Engine/Entity.h>
#include <Engine/EntityManager.h>
#include <Engine/ReddyEngine.h>
#include <Engine/SpriteComponent.h>

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
        for (const auto& pChild : children)
            drawEntitySceneTree(pChild);
        ImGui::TreePop();
    }
}

void EditorState::drawSceneUI()
{
    // Scene tree
    if (ImGui::Begin("Scene"))
    {
        //const auto& children = Engine::getEntityManager()->getRoot()->getChildren();
        //for (const auto& pChild : children)
        //    drawEntitySceneTree(pChild);
        drawEntitySceneTree(Engine::getEntityManager()->getRoot());
    }
    ImGui::End();

    // Layers
    if (ImGui::Begin("Layers"))
    {
    }
    ImGui::End();

    // Inspector (For selected entity/entities)
    if (ImGui::Begin("Entity Inspector"))
    {
        // For now, we only do 1 entity
        if (m_selected.size() == 1)
        {
            if (m_selected.front()->edit())
            {
            }
        }
    }
    ImGui::End();
}
