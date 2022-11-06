#include "Engine/Scene.h"


namespace Engine
{
    namespace Scene
    {
        Json::Value serialize()
        {
            return Json::Value(Json::objectValue);
        }

        void deserialize(const Json::Value& json)
        {
            // Clear engine of all previously loaded stuff
            // ...

            // Deserialize scene
            // ...
        }
    }
}
