#include "EditorState.h"

#include <Engine/Entity.h>

#include <imgui.h>


void EditorState::drawSceneUI()
{
    // Scene tree
    if (ImGui::Begin("Scene"))
    {
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
