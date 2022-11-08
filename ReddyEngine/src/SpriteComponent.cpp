#include "Engine/SpriteComponent.h"
#include "Engine/Utils.h"
#include "Engine/Texture.h"
#include "Engine/ResourceManager.h"
#include "Engine/ReddyEngine.h"


namespace Engine
{
    void SpriteComponent::onEnable()
    {
        // Register
    }

    void SpriteComponent::onDisable()
    {
        // Deregister
    }

    Json::Value SpriteComponent::serialize()
    {
        auto json = Component::serialize();

        json["texture"] = m_pTexture ? m_pTexture->getFilename() : "";
        json["color"] = Utils::serializeJsonValue(m_color);
        json["origin"] = Utils::serializeJsonValue(m_origin);

        return json;
    }

    void SpriteComponent::deserialize(const Json::Value& json)
    {
        Component::deserialize(json);

        m_pTexture = getResourceManager()->getTexture(Utils::deserializeString("texture"));
        const float DEFAULT_COLOR[4] = {1, 1, 1, 1};
        Utils::deserializeFloat4(&m_color.r, json["color"], DEFAULT_COLOR);
        const float DEFAULT_ORIGIN[2] = {0.5f, 0.5f};
        Utils::deserializeFloat2(&m_origin.r, json["origin"], DEFAULT_ORIGIN);
    }
}
