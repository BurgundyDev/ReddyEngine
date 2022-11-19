#include "Engine/FrameAnim.h"
#include "Engine/Log.h"
#include "Engine/ReddyEngine.h"
#include "Engine/ResourceManager.h"

namespace Engine
{
    FrameAnimRef FrameAnim::createFromFile(const std::string& filename)
    {
        // load from JSON file
        Json::Value json;
        if (!Engine::Utils::loadJson(json, filename))
        {
            CORE_ERROR("Failed to load file: %s", filename.c_str());
            tinyfd_messageBox("Error", ("Failed to load file: " + filename).c_str(), "ok", "error", 0);
            return nullptr;
        }

        return createFromJson(json);
    }
    
    FrameAnimRef FrameAnim::createFromJson(const Json::Value& json)
    {
        FrameAnimRef frameAnim;

        AnimationSet animationSet;

        for (Json::ArrayIndex index = 0; index < json["animations"].size(); index++) {
            const auto& anim = json["animations"].get(index, Json::Value { });
            std::vector<Frame> frames;

            const std::string animationName = Utils::deserializeString(anim["name"]);

            for (Json::ArrayIndex index = 0; index < anim["frames"].size(); index++) {
                const auto& frame = anim["frames"].get(index, Json::Value { });
                
                const std::string texturePath = Utils::deserializeString(frame["texture"]);
                const float frameLength = Utils::deserializeFloat(frame["length"], 1.0f);

                frames.push_back(Frame {
                    getResourceManager()->getTexture(texturePath),
                    frameLength
                });
            }

            animationSet[animationName] = Animation {
                std::move(frames)
            };
        }

        frameAnim.reset(new FrameAnim(animationSet));

        return frameAnim;
    }
}
