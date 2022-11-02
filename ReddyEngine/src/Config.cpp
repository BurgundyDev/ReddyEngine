#include "Engine/Config.h"
#include "Engine/Utils.h"


namespace Engine
{
    namespace Config
    {
        glm::ivec2 resolution = { 1280, 720 };

        void save()
        {
            auto filename = Utils::getSavePath("REDDY") + "config.json";
        }

        void load()
        {
            auto filename = Utils::getSavePath("REDDY") + "config.json";
        }
    }
}
