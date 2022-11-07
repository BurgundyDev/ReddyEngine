#include "EditorState.h"
#include "ActionManager.h"

#include <Engine/PFX.h>
#include <Engine/ReddyEngine.h>
#include <Engine/ResourceManager.h>
#include <Engine/Texture.h>

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>


namespace ImGui
{
    // Button to close a window
    bool UpButton(ImGuiID id, const ImVec2& pos)
    {
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;

        // Tweak 1: Shrink hit-testing area if button covers an abnormally large proportion of the visible region. That's in order to facilitate moving the window away. (#3825)
        // This may better be applied as a general hit-rect reduction mechanism for all widgets to ensure the area to move window is always accessible?
        const ImRect bb(pos, pos + ImVec2(g.FontSize, g.FontSize) + g.Style.FramePadding * 2.0f);
        ImRect bb_interact = bb;
        const float area_to_visible_ratio = window->OuterRectClipped.GetArea() / bb.GetArea();
        if (area_to_visible_ratio < 1.5f)
            bb_interact.Expand(ImFloor(bb_interact.GetSize() * -0.25f));

        // Tweak 2: We intentionally allow interaction when clipped so that a mechanical Alt,Right,Activate sequence can always close a window.
        // (this isn't the regular behavior of buttons, but it doesn't affect the user much because navigation tends to keep items visible).
        bool is_clipped = !ItemAdd(bb_interact, id);

        bool hovered, held;
        bool pressed = ButtonBehavior(bb_interact, id, &hovered, &held);
        if (is_clipped)
            return pressed;

        // Render
        // FIXME: Clarify this mess
        ImU32 col = GetColorU32(held ? ImGuiCol_ButtonActive : ImGuiCol_ButtonHovered);
        ImVec2 center = bb.GetCenter();
        if (hovered)
            window->DrawList->AddCircleFilled(center, ImMax(2.0f, g.FontSize * 0.5f + 1.0f), col, 12);

        float cross_extent = g.FontSize * 0.5f * 0.7071f - 1.0f;
        ImU32 cross_col = GetColorU32(ImGuiCol_Text);
        center -= ImVec2(0.5f, 0.5f);
        window->DrawList->AddLine(center + ImVec2(0, -cross_extent * 0.5f), center + ImVec2(-cross_extent, cross_extent * 0.5f), cross_col, 1.0f);
        window->DrawList->AddLine(center + ImVec2(0, -cross_extent * 0.5f), center + ImVec2(+cross_extent, cross_extent * 0.5f), cross_col, 1.0f);

        return pressed;
    }

    // Button to close a window
    bool DownButton(ImGuiID id, const ImVec2& pos)
    {
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;

        // Tweak 1: Shrink hit-testing area if button covers an abnormally large proportion of the visible region. That's in order to facilitate moving the window away. (#3825)
        // This may better be applied as a general hit-rect reduction mechanism for all widgets to ensure the area to move window is always accessible?
        const ImRect bb(pos, pos + ImVec2(g.FontSize, g.FontSize) + g.Style.FramePadding * 2.0f);
        ImRect bb_interact = bb;
        const float area_to_visible_ratio = window->OuterRectClipped.GetArea() / bb.GetArea();
        if (area_to_visible_ratio < 1.5f)
            bb_interact.Expand(ImFloor(bb_interact.GetSize() * -0.25f));

        // Tweak 2: We intentionally allow interaction when clipped so that a mechanical Alt,Right,Activate sequence can always close a window.
        // (this isn't the regular behavior of buttons, but it doesn't affect the user much because navigation tends to keep items visible).
        bool is_clipped = !ItemAdd(bb_interact, id);

        bool hovered, held;
        bool pressed = ButtonBehavior(bb_interact, id, &hovered, &held);
        if (is_clipped)
            return pressed;

        // Render
        // FIXME: Clarify this mess
        ImU32 col = GetColorU32(held ? ImGuiCol_ButtonActive : ImGuiCol_ButtonHovered);
        ImVec2 center = bb.GetCenter();
        if (hovered)
            window->DrawList->AddCircleFilled(center, ImMax(2.0f, g.FontSize * 0.5f + 1.0f), col, 12);

        float cross_extent = g.FontSize * 0.5f * 0.7071f - 1.0f;
        ImU32 cross_col = GetColorU32(ImGuiCol_Text);
        center -= ImVec2(0.5f, 0.5f);
        window->DrawList->AddLine(center + ImVec2(0, cross_extent * 0.5f), center + ImVec2(-cross_extent, -cross_extent * 0.5f), cross_col, 1.0f);
        window->DrawList->AddLine(center + ImVec2(0, cross_extent * 0.5f), center + ImVec2(+cross_extent, -cross_extent * 0.5f), cross_col, 1.0f);

        return pressed;
    }

    // p_visible == NULL                        : regular collapsing header
    // p_visible != NULL && *p_visible == true  : show a small close button on the corner of the header, clicking the button will set *p_visible = false
    // p_visible != NULL && *p_visible == false : do not show the header at all
    // Do not mistake this with the Open state of the header itself, which you can adjust with SetNextItemOpen() or ImGuiTreeNodeFlags_DefaultOpen.
    // Returns 0 if collapsed, -1 if moving up, 1 if moving down, 2 if open. (Kind of hacky, but it works)
    int CollapsingHeaderWithUpDown(const char* label, bool* p_visible = 0, ImGuiTreeNodeFlags flags = 0)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        if (p_visible && !*p_visible)
            return false;

        ImGuiID id = window->GetID(label);
        flags |= ImGuiTreeNodeFlags_CollapsingHeader;
        if (p_visible)
            flags |= ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_ClipLabelForTrailingButton;
        bool is_open = TreeNodeBehavior(id, flags, label);
        int ret = 0;
        if (p_visible != NULL)
        {
            {
                // Create a small overlapping close button
                // FIXME: We can evolve this into user accessible helpers to add extra buttons on title bars, headers, etc.
                // FIXME: CloseButton can overlap into text, need find a way to clip the text somehow.
                ImGuiContext& g = *GImGui;
                ImGuiLastItemData last_item_backup = g.LastItemData;
                float button_size = g.FontSize;
                float button_x = ImMax(g.LastItemData.Rect.Min.x, g.LastItemData.Rect.Max.x - g.Style.FramePadding.x * 2.0f - button_size);
                float button_y = g.LastItemData.Rect.Min.y;
                ImGuiID close_button_id = GetIDWithSeed("#CLOSE", NULL, id);
                if (CloseButton(close_button_id, ImVec2(button_x, button_y)))
                    *p_visible = false;
                g.LastItemData = last_item_backup;
            }
            {
                // Create a small overlapping close button
                // FIXME: We can evolve this into user accessible helpers to add extra buttons on title bars, headers, etc.
                // FIXME: CloseButton can overlap into text, need find a way to clip the text somehow.
                ImGuiContext& g = *GImGui;
                ImGuiLastItemData last_item_backup = g.LastItemData;
                float button_size = g.FontSize;
                float button_x = ImMax(g.LastItemData.Rect.Min.x, g.LastItemData.Rect.Max.x - g.Style.FramePadding.x * 2.0f - button_size);
                float button_y = g.LastItemData.Rect.Min.y;
                ImGuiID close_button_id = GetIDWithSeed("#MOVDOWN", NULL, id);
                if (DownButton(close_button_id, ImVec2(button_x - 20, button_y)))
                    ret = 1;
                g.LastItemData = last_item_backup;
            }
            {
                // Create a small overlapping close button
                // FIXME: We can evolve this into user accessible helpers to add extra buttons on title bars, headers, etc.
                // FIXME: CloseButton can overlap into text, need find a way to clip the text somehow.
                ImGuiContext& g = *GImGui;
                ImGuiLastItemData last_item_backup = g.LastItemData;
                float button_size = g.FontSize;
                float button_x = ImMax(g.LastItemData.Rect.Min.x, g.LastItemData.Rect.Max.x - g.Style.FramePadding.x * 2.0f - button_size);
                float button_y = g.LastItemData.Rect.Min.y;
                ImGuiID close_button_id = GetIDWithSeed("#MOVUP", NULL, id);
                if (UpButton(close_button_id, ImVec2(button_x - 40, button_y)))
                    ret = -1;
                g.LastItemData = last_item_backup;
            }
        }

        return ret ? ret : (is_open ? 2 : 0);
    }
}


void EditorState::drawPFXUI()
{
    if (ImGui::Begin("PFX Inspector"))
    {
        if (ImGui::Button("Play"))
        {
            m_pPfxInstance = std::make_shared<Engine::PFXInstance>(m_pPfx);
        }
        bool changed = false;
        if (ImGui::Button("Add Emitter"))
        {
            Engine::EmitterDef emitter;
            emitter.pTexture = Engine::getResourceManager()->getTexture("textures/particle.png");
            m_pPfx->emitters.push_back(emitter);
            changed = true;
        }
        int i = 0;
        for (auto it = m_pPfx->emitters.begin(); it != m_pPfx->emitters.end(); )
        {
            auto& emitter = *it;

            ++i;
            bool open = true;
            int nav = 0;
            if ((nav = ImGui::CollapsingHeaderWithUpDown((emitter.name + "##emitter" + std::to_string(i)).c_str(), &open)) == 2)
            {
                ImGui::PushID(i);
                ImGui::Indent(10.0f);

                {
                    char buf[260];
                    memcpy(buf, emitter.name.c_str(), emitter.name.size() + 1);
                    ImGui::InputText("Name", buf, 260);
                    if (ImGui::IsItemDeactivatedAfterEdit())
                    {
                        emitter.name = buf;
                        changed = true;
                    }
                }
                
                {
                    const char* EMITTER_TYPE_CHOICES[] = { "burst", "continuous" };
                    int current_choice = (int)emitter.type;
                    if (ImGui::Combo("Type", &current_choice, "burst\0continuous"))
                    {
                        changed |= (emitter.type != (Engine::EmitterType)current_choice);
                        emitter.type = (Engine::EmitterType)current_choice;
                    }
                }

                switch (emitter.type)
                {
                    case Engine::EmitterType::burst:
                        ImGui::DragFloat("Burst Duration", &emitter.burstDuration, 0.01f, 0.0f, 60.0f * 20.0f);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                        ImGui::DragInt("Burst Amount", &emitter.burstAmount, 1.0f, 1, 1000);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                        break;
                    case Engine::EmitterType::continuous:
                        ImGui::DragFloat("Spawn Rate", &emitter.spawnRate);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                        break;
                }

                {
                    char buf[260];
                    if (emitter.pTexture)
                        memcpy(buf, emitter.pTexture->getFilename().c_str(), emitter.pTexture->getFilename().size() + 1);
                    else
                        buf[0] = '\0';
                    ImGui::InputText("Texture", buf, 260);
                    if (ImGui::IsItemDeactivatedAfterEdit())
                    {
                        emitter.pTexture = Engine::getResourceManager()->getTexture(buf);
                        changed = true;
                    }
                }

                ImGui::SliderFloat("Spread", &emitter.spread, 0.0f, 360.0f);

                // Color
                {
                    ImGui::Separator();
                    ImGui::Text("Color");
                    ImGui::Indent(10.0f);

                    if (ImGui::Checkbox("Same Start-End##colorsamestartend", &emitter.color.sameStartEnd)) changed = true;
                    if (ImGui::Checkbox("Random Start##colorrandomstart", &emitter.color.randomStart)) changed = true;
                    ImGui::ColorEdit4("Start Color 1", &emitter.color.startRange[0].x);
                    if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    if (emitter.color.randomStart)
                    {
                        ImGui::ColorEdit4("Start Color 2", &emitter.color.startRange[1].x);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    }
                    if (!emitter.color.sameStartEnd)
                    {
                        if (ImGui::Checkbox("Random End##colorrandomend", &emitter.color.randomEnd)) changed = true;
                        ImGui::ColorEdit4("End Color 1", &emitter.color.endRange[0].x);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                        if (emitter.color.randomEnd)
                        {
                            ImGui::ColorEdit4("End Color 2", &emitter.color.endRange[1].x);
                            if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                        }
                        ImGui::Checkbox("End only affect Alpha", &emitter.endOnlyAffectAlpha);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    }

                    ImGui::Unindent(10.0f);
                }

                // additive
                {
                    ImGui::Separator();
                    ImGui::Text("Additive");
                    ImGui::Indent(10.0f);
                    
                    if (ImGui::Checkbox("Same Start-End##additivesamestartend", &emitter.additive.sameStartEnd)) changed = true;
                    if (ImGui::Checkbox("Random Start##additiverandomstart", &emitter.additive.randomStart)) changed = true;
                    ImGui::SliderFloat("Start Additive 1", &emitter.additive.startRange[0], 0.0f, 1.0f);
                    if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    if (emitter.additive.randomStart)
                    {
                        ImGui::SliderFloat("Start Additive 2", &emitter.additive.startRange[1], 0.0f, 1.0f);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    }
                    if (!emitter.additive.sameStartEnd)
                    {
                        if (ImGui::Checkbox("Random End##additiverandomend", &emitter.additive.randomEnd)) changed = true;
                        ImGui::SliderFloat("End Additive 1", &emitter.additive.endRange[0], 0.0f, 1.0f);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                        if (emitter.additive.randomEnd)
                        {
                            ImGui::SliderFloat("End Additive 2", &emitter.additive.endRange[1], 0.0f, 1.0f);
                            if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                        }
                    }

                    ImGui::Unindent(10.0f);
                }

                // size
                {
                    ImGui::Separator();
                    ImGui::Text("Size");
                    ImGui::Indent(10.0f);
                    
                    if (ImGui::Checkbox("Same Start-End##sizesamestartend", &emitter.size.sameStartEnd)) changed = true;
                    if (ImGui::Checkbox("Random Start##sizerandomstart", &emitter.size.randomStart)) changed = true;
                    ImGui::DragFloat("Start Size 1", &emitter.size.startRange[0], 0.1f);
                    if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    if (emitter.size.randomStart)
                    {
                        ImGui::DragFloat("Start Size 2", &emitter.size.startRange[1], 0.1f);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    }
                    if (!emitter.size.sameStartEnd)
                    {
                        if (ImGui::Checkbox("Random End##sizerandomend", &emitter.size.randomEnd)) changed = true;
                        ImGui::DragFloat("End Size 1", &emitter.size.endRange[0], 0.1f);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                        if (emitter.size.randomEnd)
                        {
                            ImGui::DragFloat("End Size 2", &emitter.size.endRange[1], 0.1f);
                            if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                        }
                    }

                    ImGui::Unindent(10.0f);
                }

                // Rotation
                {
                    ImGui::Separator();
                    ImGui::Text("Rotation");
                    ImGui::Indent(10.0f);
                    
                    if (ImGui::Checkbox("Random##rotrandom", &emitter.rotation.random)) changed = true;
                    ImGui::DragFloat("Rot 1", &emitter.rotation.range[0]);
                    if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    if (emitter.rotation.random)
                    {
                        ImGui::DragFloat("Rot 2", &emitter.rotation.range[1]);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    }

                    ImGui::Unindent(10.0f);
                }

                // Rotation Speed
                {
                    ImGui::Separator();
                    ImGui::Text("Rotation Speed");
                    ImGui::Indent(10.0f);
                    
                    if (ImGui::Checkbox("Same Start-End##rotationspeedsamestartend", &emitter.rotationSpeed.sameStartEnd)) changed = true;
                    if (ImGui::Checkbox("Random Start##rotspeedrandomstart", &emitter.rotationSpeed.randomStart)) changed = true;
                    ImGui::DragFloat("Start Rot Speed 1", &emitter.rotationSpeed.startRange[0], 0.1f);
                    if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    if (emitter.rotationSpeed.randomStart)
                    {
                        ImGui::DragFloat("Start Rot Speed 2", &emitter.rotationSpeed.startRange[1], 0.1f);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    }
                    if (!emitter.rotationSpeed.sameStartEnd)
                    {
                        if (ImGui::Checkbox("Random End##rotspeedrandomend", &emitter.rotationSpeed.randomEnd)) changed = true;
                        ImGui::DragFloat("End Rot Speed 1", &emitter.rotationSpeed.endRange[0], 0.1f);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                        if (emitter.rotationSpeed.randomEnd)
                        {
                            ImGui::DragFloat("End Rot Speed 2", &emitter.rotationSpeed.endRange[1], 0.1f);
                            if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                        }
                    }

                    ImGui::Unindent(10.0f);
                }

                // Speed
                {
                    ImGui::Separator();
                    ImGui::Text("Speed");
                    ImGui::Indent(10.0f);
                    
                    if (ImGui::Checkbox("Same Start-End##speedsamestartend", &emitter.speed.sameStartEnd)) changed = true;
                    if (ImGui::Checkbox("Random Start##speedrandomstart", &emitter.speed.randomStart)) changed = true;
                    ImGui::DragFloat("Start Speed 1", &emitter.speed.startRange[0]);
                    if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    if (emitter.speed.randomStart)
                    {
                        ImGui::DragFloat("Start Speed 2", &emitter.speed.startRange[1]);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    }
                    if (!emitter.speed.sameStartEnd)
                    {
                        if (ImGui::Checkbox("Random End##speedrandomend", &emitter.speed.randomEnd)) changed = true;
                        ImGui::DragFloat("End Speed 1", &emitter.speed.endRange[0]);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                        if (emitter.speed.randomEnd)
                        {
                            ImGui::DragFloat("End Speed 2", &emitter.speed.endRange[1]);
                            if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                        }
                    }

                    ImGui::Unindent(10.0f);
                }

                // Duration
                {
                    ImGui::Separator();
                    ImGui::Text("Duration");
                    ImGui::Indent(10.0f);

                    if (ImGui::Checkbox("Random##durationrandom", &emitter.duration.random)) changed = true;
                    ImGui::DragFloat("Duration 1", &emitter.duration.range[0]);
                    if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    if (emitter.duration.random)
                    {
                        ImGui::DragFloat("Duration 2", &emitter.duration.range[1]);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    }

                    ImGui::Unindent(10.0f);
                }

                // Gravity
                {
                    ImGui::Separator();
                    ImGui::Text("Gravity");
                    ImGui::Indent(10.0f);
                    
                    if (ImGui::Checkbox("Same Start-End##gravitysamestartend", &emitter.gravity.sameStartEnd)) changed = true;
                    if (ImGui::Checkbox("Random Start##gravityrandomstart", &emitter.gravity.randomStart)) changed = true;
                    ImGui::DragFloat2("Start Grav 1", &emitter.gravity.startRange[0].x);
                    if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    if (emitter.gravity.randomStart)
                    {
                        ImGui::DragFloat2("Start Grav 2", &emitter.gravity.startRange[1].x);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    }
                    if (!emitter.gravity.sameStartEnd)
                    {
                        if (ImGui::Checkbox("Random End##gravityrandomend", &emitter.gravity.randomEnd)) changed = true;
                        ImGui::DragFloat2("End Grav 1", &emitter.gravity.endRange[0].x);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                        if (emitter.gravity.randomEnd)
                        {
                            ImGui::DragFloat2("End Grav 2", &emitter.gravity.endRange[1].x);
                            if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                        }
                    }

                    ImGui::Unindent(10.0f);
                }

                // Spawn Radius
                {
                    ImGui::Separator();
                    ImGui::Text("Spawn Radius");
                    ImGui::Indent(10.0f);

                    if (ImGui::Checkbox("Random##spawnradiusrandom", &emitter.spawnRadius.random)) changed = true;
                    ImGui::DragFloat("Radius 1", &emitter.spawnRadius.range[0]);
                    if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    if (emitter.spawnRadius.random)
                    {
                        ImGui::DragFloat("Radius 2", &emitter.spawnRadius.range[1]);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    }

                    ImGui::Unindent(10.0f);
                }

                ImGui::Unindent(10.0f);
                ImGui::PopID();
            }

            if (!open)
            {
                changed = true;
                it = m_pPfx->emitters.erase(it);
                continue;
            }

            if (nav == -1)
            {
                if (i > 1)
                {
                    changed = true;
                    auto copy = emitter;
                    it = m_pPfx->emitters.erase(it);
                    it = m_pPfx->emitters.insert(it - 1, copy) + 1;
                    continue;
                }
            }

            if (nav == 1)
            {
                if (i < (int)m_pPfx->emitters.size())
                {
                    changed = true;
                    auto copy = emitter;
                    it = m_pPfx->emitters.erase(it);
                    it = m_pPfx->emitters.insert(it + 1, copy) + 1;
                    continue;
                }
            }

            ++it;
        }

        if (changed)
        {
            setDirty(true);
            auto prevJson = m_pfxJson;
            m_pfxJson = m_pPfx->serialize();
            auto newJson = m_pfxJson;
            m_pActionManager->addAction("Edit PFX", 
                                        [this, newJson]() // Redo
            {
                m_pPfxInstance.reset();
                m_pPfx->deserialize(newJson);
                m_pfxJson = newJson;
                setDirty(true);
            },
                                        [this, prevJson]() // Undo
            {
                m_pPfxInstance.reset();
                m_pPfx->deserialize(prevJson);
                m_pfxJson = prevJson;
                setDirty(true);
            });
        }
    }
    ImGui::End();
}
