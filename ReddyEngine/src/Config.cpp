#include "Engine/Config.h"
#include "Engine/Log.h"
#include "Engine/Utils.h"

#include <json/json.h>

#include <fstream>


namespace Engine
{
    namespace Config
    {
#if defined(FINAL)
        glm::ivec2 resolution = { 1280, 720 };
        bool dpiAware = true;
        bool showFPS = false;
        DisplayMode displayMode = DisplayMode::BorderlessFullscreen;
#else
        glm::ivec2 resolution = { 1280, 720 };
        bool dpiAware = true;
        bool showFPS = true;
        DisplayMode displayMode = DisplayMode::Windowed;
#endif
        bool vsync = true;
        float masterVolume = 1.0f;
        float sfxVolume = 1.0f;
        float musicVolume = 1.0f;
        std::vector<std::string> recentEditorFiles;

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
            json["recentEditorFiles"] = Utils::serializeStringArray(recentEditorFiles);
            json["displayMode"] = Utils::serializeInt32((int32_t)displayMode);
            json["dpiAware"] = Utils::serializeBool(dpiAware);
            json["showFPS"] = Utils::serializeBool(showFPS);

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
                recentEditorFiles = Utils::deserializeStringArray(json["recentEditorFiles"]);
                displayMode = (DisplayMode)Utils::deserializeInt32(json["displayMode"], (int32_t)displayMode);
                dpiAware = Utils::deserializeBool(json["dpiAware"], dpiAware);
                showFPS = Utils::deserializeBool(json["showFPS"], showFPS);
            }

            configJsonAtLaunch = json;
        }
    }
}
