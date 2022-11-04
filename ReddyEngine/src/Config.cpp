#include "Engine/Config.h"
#include "Engine/Log.h"
#include "Engine/Utils.h"

#include <json/json.h>

#include <fstream>


namespace Engine
{
    namespace Config
    {
        glm::ivec2 resolution = { 1280, 720 };
        bool vsync = true;
        float masterVolume = 1.0f;
        float sfxVolume = 1.0f;
        float musicVolume = 1.0f;

        Json::Value configJsonAtLaunch;


        void save()
        {
            auto filename = Utils::getSavePath("REDDY") + "config.json";

            Json::Value json;
            json["resolution"] = Utils::serializeInt2(&resolution.x);
            json["vsync"] = Utils::serializeBool(vsync);
            json["masterVolume"] = Utils::serializeFloat(masterVolume);
            json["sfxVolume"] = Utils::serializeFloat(sfxVolume);
            json["musicVolume"] = Utils::serializeFloat(musicVolume);

            if (json == configJsonAtLaunch)
            {
                // Save config as at launch, don't resave
                CORE_INFO("Configs haven't changed, skip saving");
                return;
            }

            Utils::saveJson(json, filename);
        }


        void load()
        {
            auto filename = Utils::getSavePath("REDDY") + "config.json";

            Json::Value json;
            if (Utils::loadJson(json, filename))
            {
                Utils::deserializeInt2(&resolution.x, json["resolution"], &resolution.x);
                vsync = Utils::deserializeBool(json["vsync"], vsync);
                masterVolume = Utils::deserializeBool(json["masterVolume"], masterVolume);
                sfxVolume = Utils::deserializeBool(json["sfxVolume"], sfxVolume);
                musicVolume = Utils::deserializeBool(json["musicVolume"], musicVolume);
            }

            configJsonAtLaunch = json;
        }
    }
}
