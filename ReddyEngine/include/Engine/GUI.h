#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <memory>
#include <string>


namespace Engine
{
    class Texture;
    using TextureRef = std::shared_ptr<Texture>;


    namespace GUI
    {
        enum class SectionState
        {
            Collapsed = 0,
            Open = 2,
            MoveUp = -1,
            MoveDown = 1,
            Delete = 3
        };

        bool beginEditorWindow(const std::string& name);
        void endEditorWindow();

        void statusBarText(const std::string& text);
        bool statusBarButton(const std::string& text, const char* tooltip = nullptr);

        SectionState beginSection(const std::string& title);
        void endSection(); // Only call if beginSection returned SectionState::Open

        void beginGroup(const char* title);
        void endGroup();

        // Properties. Return true if it has changed (Needs undo)
        bool stringProperty(const char* label, std::string* value);
        bool enumProperty(const char* label, int* choice, const char* items_separated_by_zeros);
        template<typename T> bool enumProperty(const char* label, T* choice, const char* items_separated_by_zeros) { return enumProperty(label, (int*)choice, items_separated_by_zeros); }
        bool floatProperty(const char* label, float* value, float min = -1000000.0f, float max = 1000000.0f);
        bool floatSliderProperty(const char* label, float* value, float min, float max);
        bool intProperty(const char* label, int* value, int min = -1000000, int max = 1000000);
        bool textureProperty(const char* label, TextureRef& value);
        bool boolProperty(const char* label, bool* value);
        bool colorProperty(const char* label, glm::vec4* color);
        bool vec2Property(const char* label, glm::vec2* value);
    }
}
