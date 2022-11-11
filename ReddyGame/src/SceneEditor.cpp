#include "EditorState.h"
#include "ActionManager.h"

#include <Engine/Entity.h>
#include <Engine/Scene.h>
#include <Engine/ReddyEngine.h>
#include <Engine/SpriteComponent.h>
#include <Engine/GUI.h>

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

void EditorState::drawSceneUI()
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
