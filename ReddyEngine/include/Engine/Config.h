#pragma once

#include <glm/vec2.hpp>

#include <string>
#include <vector>


namespace Engine
{
    namespace Config
    {
        enum class DisplayMode : int
        {
            Windowed = 0,
            BorderlessFullscreen = 1
        };

        extern glm::ivec2 resolution;
        extern bool vsync;
        extern bool showFPS;
        extern DisplayMode displayMode;
        extern float masterVolume;
        extern float sfxVolume;
        extern float musicVolume;
        extern std::vector<std::string> recentEditorFiles;
        extern bool dpiAware;

        void load();
        void save();
    }
}
