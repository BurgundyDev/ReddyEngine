#pragma once

#include <glm/vec2.hpp>


namespace Engine
{
    namespace Config
    {
        extern glm::ivec2 resolution;

        void load();
        void save();
    }
}
