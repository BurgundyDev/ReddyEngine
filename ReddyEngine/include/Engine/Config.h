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
            Windowed,
            BorderlessFullscreen
        };

        extern glm::ivec2 resolution;
        extern bool vsync;
        //extern DisplayMode displayMode;
        extern float masterVolume;
        extern float sfxVolume;
        extern float musicVolume;
        extern std::vector<std::string> recentEditorFiles;

        void load();
        void save();
    }
}
