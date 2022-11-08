#include "Engine/Scene.h"
#include "Engine/EntityManager.h"
#include "Engine/ReddyEngine.h"


namespace Engine
{
    namespace Scene
    {
        void clear()
        {
            getEntityManager()->clear();
        }

        Json::Value serialize()
        {
            Json::Value json;
            json["root"] = getEntityManager()->serialize();
            return json;
        }

        void deserialize(const Json::Value& json)
        {
            getEntityManager()->deserialize(json["root"]);
        }
    }
}
