#include "Engine/SpriteComponent.h"
#include "Engine/Utils.h"
#include "Engine/Texture.h"
#include "Engine/ResourceManager.h"
#include "Engine/ReddyEngine.h"
#include "Engine/Constants.h"
#include "Engine/GUI.h"


namespace Engine
{
    SpriteComponent::SpriteComponent()
    {
        pTexture = getResourceManager()->getTexture("textures/defaultTexture.png");
    }

    Json::Value SpriteComponent::serialize()
    {
        auto json = Component::serialize();

        json["texture"] = pTexture ? pTexture->getFilename() : "";
        json["color"] = Utils::serializeJsonValue(color);
        json["origin"] = Utils::serializeJsonValue(origin);
        json["uvs"] = Utils::serializeJsonValue(uvs);

        return json;
    }

    void SpriteComponent::deserialize(const Json::Value& json)
    {
        Component::deserialize(json);

        pTexture = getResourceManager()->getTexture(Utils::deserializeString(json["texture"]));

        const float DEFAULT_COLOR[4] = {1, 1, 1, 1};
        Utils::deserializeFloat4(&color.r, json["color"], DEFAULT_COLOR);

        const float DEFAULT_ORIGIN[2] = {0.5f, 0.5f};
        Utils::deserializeFloat2(&origin.x, json["origin"], DEFAULT_ORIGIN);

        const float DEFAULT_UVS[4] = {0, 0, 1, 1};
        Utils::deserializeFloat4(&uvs.x, json["uvs"], DEFAULT_UVS);
    }

    bool SpriteComponent::edit()
    {
        bool changed = false;

        changed |= GUI::textureProperty("Texture", &pTexture);
        changed |= GUI::colorProperty("Color", &color);
        changed |= GUI::originProperty("Origin", &origin);

        return changed;
    }
}
