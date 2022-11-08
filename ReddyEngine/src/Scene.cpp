#include "Engine/Scene.h"
#include "Engine/EntityManager.h"
#include "Engine/ReddyEngine.h"
#include "Engine/SpriteManager.h"
#include "Engine/SpriteBatch.h"


namespace Engine
{
    namespace Scene
    {
        void clear()
        {
            getEntityManager()->clear();
            getSpriteManager()->clear();
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

        void update(float dt)
        {
            getEntityManager()->update(dt);
        }

        void fixedUpdate(float dt)
        {
            getEntityManager()->fixedUpdate(dt);
        }

        void draw()
        {
            getSpriteManager()->draw();
        }
    }
}
