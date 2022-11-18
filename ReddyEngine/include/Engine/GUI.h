#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <memory>
#include <string>


namespace Engine
{
    class Texture;
    using TextureRef = std::shared_ptr<Texture>;

    class Font;
    using FontRef = std::shared_ptr<Font>;

    class PFX;
    using PFXRef = std::shared_ptr<PFX>;


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
        bool stringProperty(const char* label, std::string* value, const char* tooltip = nullptr);
        bool multilineStringProperty(const char* label, std::string* value, const char* tooltip = nullptr);
        bool enumProperty(const char* label, int* choice, const char* items_separated_by_zeros, const char* tooltip = nullptr);
        template<typename T> bool enumProperty(const char* label, T* choice, const char* items_separated_by_zeros, const char* tooltip = nullptr) { return enumProperty(label, (int*)choice, items_separated_by_zeros, tooltip); }
        bool floatProperty(const char* label, float* value, float min = -1000000.0f, float max = 1000000.0f, const char* tooltip = nullptr);
        bool angleProperty(const char* label, float* value, const char* tooltip = nullptr);
        bool floatSliderProperty(const char* label, float* value, float min, float max, const char* tooltip = nullptr);
        bool intProperty(const char* label, int* value, int min = -1000000, int max = 1000000, const char* tooltip = nullptr);
        bool boolProperty(const char* label, bool* value, const char* tooltip = nullptr);
        bool colorProperty(const char* label, glm::vec4* color, const char* tooltip = nullptr);
        bool vec2Property(const char* label, glm::vec2* value, const char* tooltip = nullptr);
        bool originProperty(const char* label, glm::vec2* value, const char* tooltip = nullptr);
        void idProperty(const char* label, uint64_t id, const char* tooltip = nullptr); // Can't modify this

        // Resources
        bool textureProperty(const char* label, TextureRef* value, const char* tooltip = nullptr);
        bool fontProperty(const char* label, FontRef* value, const char* tooltip = nullptr);
        bool PFXProperty(const char* label, PFXRef* value, const char* tooltip = nullptr);
    }
}
