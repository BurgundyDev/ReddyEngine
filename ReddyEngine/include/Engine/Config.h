#pragma once

#include <glm/vec2.hpp>


namespace Engine
{
    namespace Config
    {
        extern glm::ivec2 resolution;
        extern bool vsync;
        extern float masterVolume;
        extern float sfxVolume;
        extern float musicVolume;

        void load();
        void save();
    }
}
