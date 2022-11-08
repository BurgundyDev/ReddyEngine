#pragma once

#include <json/json.h>


namespace Engine
{
    namespace Scene
    {
        void clear();
        Json::Value serialize();
        void deserialize(const Json::Value& json);
    }
}
