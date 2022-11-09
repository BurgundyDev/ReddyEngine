#pragma once

#include <json/json.h>
#include <glm/vec2.hpp>


namespace Engine
{
    namespace Scene
    {
        void clear();
        Json::Value serialize();
        void deserialize(const Json::Value& json);
        void update(float dt);
        void fixedUpdate(float dt);
        void draw();
    }
}
