#pragma once

#include <json/json.h>


namespace Engine
{
    namespace Scene
    {
        Json::Value serialize();
        void deserialize(const Json::Value& json);
    }
}
