#include "Engine/GUI.h"
#include "Engine/Texture.h"
#include "Engine/ResourceManager.h"
#include "Engine/ReddyEngine.h"

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
    Engine::GUI::SectionState CollapsingHeaderWithUpDown(const char* label, bool* p_visible = 0, ImGuiTreeNodeFlags flags = 0)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return Engine::GUI::SectionState::Collapsed;

        if (p_visible && !*p_visible)
            return Engine::GUI::SectionState::Collapsed;

        ImGuiID id = window->GetID(label);
        flags |= ImGuiTreeNodeFlags_CollapsingHeader;
        if (p_visible)
            flags |= ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_ClipLabelForTrailingButton;
        bool is_open = TreeNodeBehavior(id, flags, label);
        Engine::GUI::SectionState ret = is_open ? Engine::GUI::SectionState::Open : Engine::GUI::SectionState::Collapsed;
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
                    ret = Engine::GUI::SectionState::MoveDown;
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
                    ret = Engine::GUI::SectionState::MoveUp;
                g.LastItemData = last_item_backup;
            }
        }
        
        return ret;
    }
}


namespace Engine
{
    namespace GUI
    {
        static bool g_windowOpen = false;
        static bool g_statusBarDrawn = false;
        static int g_statusBarBtnCount = 0;
        static ImVec2 g_statusBarPos;
        static int g_sectionCount = 0;
        static int g_propertyCount = 0;


        static void beginStatusBar()
        {
            ImGui::EndChild();
            ImGui::Separator();
            g_statusBarDrawn = true;
            g_statusBarPos = ImGui::GetCursorPos();
        }


        static void showToolTip(const char* text)
        {
            if (!text) return;

            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
            {
                auto pos = ImGui::GetCursorPos();
                ImGui::BeginTooltip();
                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                ImGui::TextUnformatted(text);
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }
        }


        bool beginEditorWindow(const std::string& name)
        {
            g_windowOpen = ImGui::Begin(name.c_str());
            if (g_windowOpen)
            {
                // Leave space for status bar
                ImGui::BeginChild("Client Area", ImGui::GetWindowSize() - ImVec2(12, 62));
                g_statusBarDrawn = false;
                g_statusBarBtnCount = 0;
                g_sectionCount = 0;
                g_propertyCount = 0;
            }

            return g_windowOpen;
        }

        void endEditorWindow()
        {
            if (g_windowOpen && !g_statusBarDrawn) beginStatusBar();
            ImGui::End();
        }

        void statusBarText(const std::string& text)
        {
            if (!g_statusBarDrawn) beginStatusBar();

            ImGui::SetCursorPos(g_statusBarPos);
            ImGui::Text(text.c_str());
        }

        bool statusBarButton(const std::string& text, const char* tooltip)
        {
            if (!g_statusBarDrawn) beginStatusBar();

            ImGui::SetCursorPos({ImGui::GetWindowWidth() - 40 - 24 * (float)g_statusBarBtnCount++, ImGui::GetCursorPosY()});
            auto ret = ImGui::Button(text.c_str(), {20, 18});
            showToolTip(tooltip);

            return ret;
        }

        SectionState beginSection(const std::string& text)
        {
            auto sectionId = ++g_sectionCount;
            bool visible = true;
            auto ret = (SectionState)ImGui::CollapsingHeaderWithUpDown((text + "##section" + std::to_string(sectionId)).c_str(), &visible);
            if (!visible) ret = SectionState::Delete;

            if (ret == SectionState::Open)
            {
                ImGui::PushID(sectionId);
                ImGui::Indent(10.0f);
            }

            return ret;
        }

        void endSection()
        {
            ImGui::Unindent(10.0f);
            ImGui::PopID();
        }

        void beginGroup(const char* title)
        {
            ImGui::Separator();
            ImGui::Text(title);
            ImGui::Indent(10.0f);
        }

        void endGroup()
        {
            ImGui::Unindent(10.0f);
        }

        
        // Properties


        bool stringProperty(const char* label, std::string* value)
        {
            bool ret = false;
            ImGui::PushID(++g_propertyCount);

            char buf[260];
            memcpy(buf, value->c_str(), value->size() + 1);
            ImGui::InputText(label, buf, 260);
            if (ImGui::IsItemDeactivatedAfterEdit())
            {
                *value = buf;
                ret = true;
            }

            ImGui::PopID();
            return false;
        }

        bool enumProperty(const char* label, int* choice, const char* items_separated_by_zeros)
        {
            bool ret = false;
            ImGui::PushID(++g_propertyCount);

            int current_choice = *choice;
            if (ImGui::Combo(label, &current_choice, items_separated_by_zeros))
            {
                ret = ((*choice) != current_choice);
                *choice = current_choice;
            }

            ImGui::PopID();
            return ret;
        }

        bool floatProperty(const char* label, float* value, float min, float max)
        {
            ImGui::PushID(++g_propertyCount);
            ImGui::DragFloat(label, value, 0.01f, min, max);
            auto ret = ImGui::IsItemDeactivatedAfterEdit();
            ImGui::PopID();
            return ret;
        }

        bool angleProperty(const char* label, float* value)
        {
            ImGui::PushID(++g_propertyCount);
            ImGui::DragFloat(label, value, 1.0f);
            auto ret = ImGui::IsItemDeactivatedAfterEdit();
            ImGui::PopID();
            return ret;
        }

        bool floatSliderProperty(const char* label, float* value, float min, float max)
        {
            ImGui::PushID(++g_propertyCount);
            ImGui::SliderFloat(label, value, min, max);
            auto ret = ImGui::IsItemDeactivatedAfterEdit();
            ImGui::PopID();
            return ret;
        }

        bool intProperty(const char* label, int* value, int min, int max)
        {
            ImGui::PushID(++g_propertyCount);
            ImGui::DragInt(label, value, 1.0f, min, max);
            auto ret = ImGui::IsItemDeactivatedAfterEdit();
            ImGui::PopID();
            return ret;
        }

        bool textureProperty(const char* label, TextureRef* value)
        {
            bool ret = false;
            ImGui::PushID(++g_propertyCount);

            char buf[260];
            if (*value)
                memcpy(buf, (*value)->getFilename().c_str(), (*value)->getFilename().size() + 1);
            else
                buf[0] = '\0';

            ImGui::InputText(label, buf, 260);
            if (ImGui::IsItemDeactivatedAfterEdit())
            {
                *value = getResourceManager()->getTexture(buf);
                ret = true;
            }

            ImGui::PopID();
            return false;
        }

        bool boolProperty(const char* label, bool* value)
        {
            ImGui::PushID(++g_propertyCount);
            auto ret = ImGui::Checkbox(label, value);
            ImGui::PopID();
            return ret;
        }

        bool colorProperty(const char* label, glm::vec4* color)
        {
            ImGui::PushID(++g_propertyCount);
            ImGui::ColorEdit4(label, &color->r);
            auto ret = ImGui::IsItemDeactivatedAfterEdit();
            ImGui::PopID();
            return ret;
        }

        bool vec2Property(const char* label, glm::vec2* value)
        {
            ImGui::PushID(++g_propertyCount);
            ImGui::DragFloat2(label, &value->x, 0.01f);
            auto ret = ImGui::IsItemDeactivatedAfterEdit();
            ImGui::PopID();
            return ret;
        }
        
        bool originProperty(const char* label, glm::vec2* value)
        {
            //TODO: Draw a box with 9 corners to quickly pick an origin
            ImGui::PushID(++g_propertyCount);
            ImGui::DragFloat2(label, &value->x, 0.01f);
            auto ret = ImGui::IsItemDeactivatedAfterEdit();
            ImGui::PopID();
            return ret;
        }

        void idProperty(const char* label, uint64_t id)
        {
            ImGui::PushID(++g_propertyCount);
            ImGui::LabelText(label, "%llu", id);
            ImGui::PopID();
        }
    }
}
