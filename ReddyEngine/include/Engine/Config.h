#pragma once

#include <glm/vec2.hpp>

#include <string>
#include <vector>


namespace Engine
{
    namespace Config
    {
        extern glm::ivec2 resolution;
        extern bool vsync;
        extern float masterVolume;
        extern float sfxVolume;
        extern float musicVolume;
        extern std::vector<std::string> recentEditorFiles;

        void load();
        void save();
    }
}
