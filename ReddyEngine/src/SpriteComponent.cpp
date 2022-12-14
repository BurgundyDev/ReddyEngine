#include "Engine/SpriteComponent.h"
#include "Engine/Utils.h"
#include "Engine/Texture.h"
#include "Engine/ResourceManager.h"
#include "Engine/ReddyEngine.h"
#include "Engine/Constants.h"
#include "Engine/GUI.h"
#include "Engine/Entity.h"
#include "Engine/SpriteBatch.h"
#include "Engine/FrameAnimComponent.h"


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
        json["additive"] = Utils::serializeJsonValue(additive);

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

        additive = Utils::deserializeFloat(json["additive"], 0.0f);
    }

    bool SpriteComponent::edit()
    {
        bool changed = false;

        changed |= GUI::textureProperty("Texture", &pTexture, "", isFrameAnimTexture());
        changed |= GUI::colorProperty("Color", &color);
        changed |= GUI::originProperty("Origin", &origin);
        changed |= GUI::floatSliderProperty("Additive", &additive, 0.0f, 1.0f, "Alpha blend to Additive blend ratio.");

        return changed;
    }

    bool SpriteComponent::isMouseHover(const glm::vec2& mousePos) const
    {
        if (!pTexture) return Component::isMouseHover(mousePos);
        
        const auto& invTransform = m_pEntity->getInvWorldTransformWithScale();

        glm::ivec2 textureSize = pTexture ? pTexture->getSize() : glm::ivec2{ 1, 1 };
        glm::vec2 sizef = glm::vec2((float)textureSize.x, (float)textureSize.y) * SPRITE_BASE_SCALE;
        glm::vec2 invOrigin(1.f - origin.x, 1.f - origin.y);

        glm::vec2 localMouse = invTransform * glm::vec4(mousePos, 0, 1);

        return 
            localMouse.x >= -sizef.x * origin.x &&
            localMouse.x <= sizef.x * invOrigin.x &&
            localMouse.y >= -sizef.y * origin.y &&
            localMouse.y <= sizef.y * invOrigin.y;
    }

	void SpriteComponent::drawOutline(const glm::vec4& color, float zoomScale)
    {
		const auto& transform = m_pEntity->getWorldTransformWithScale();
		auto sb = getSpriteBatch().get();

        glm::ivec2 textureSize = pTexture ? pTexture->getSize() : glm::ivec2{ 1, 1 };
        glm::vec2 sizef = glm::vec2((float)textureSize.x, (float)textureSize.y) * SPRITE_BASE_SCALE;
        glm::vec2 invOrigin(1.f - origin.x, 1.f - origin.y);

        glm::vec2 points[4] = {
            transform * glm::vec4(-sizef.x * origin.x, -sizef.y * origin.y, 0, 1),
            transform * glm::vec4(-sizef.x * origin.x, sizef.y * invOrigin.y, 0, 1),
            transform * glm::vec4(sizef.x * invOrigin.x, sizef.y * invOrigin.y, 0, 1),
            transform * glm::vec4(sizef.x * invOrigin.x, -sizef.y * origin.y, 0, 1)
        };

		sb->drawLine(points[0], points[1], 2.0f * zoomScale, color);
        sb->drawLine(points[1], points[2], 2.0f * zoomScale, color);
        sb->drawLine(points[2], points[3], 2.0f * zoomScale, color);
        sb->drawLine(points[3], points[0], 2.0f * zoomScale, color);

        auto pos = m_pEntity->getWorldPosition();
		sb->drawLine(glm::vec2(pos.x, pos.y - 0.05f), glm::vec2(pos.x, pos.y + 0.05f), 2.0f * zoomScale, color);
		sb->drawLine(glm::vec2(pos.x - 0.05f, pos.y), glm::vec2(pos.x + 0.05f, pos.y), 2.0f * zoomScale, color);
    }

    TextureRef SpriteComponent::getEditorIcon() const
    {
        return pTexture ? pTexture : Component::getEditorIcon();
    }

    bool SpriteComponent::isFrameAnimTexture() const
    {
        return pTexture != nullptr && m_pEntity->hasComponent<FrameAnimComponent>();
    }

    std::string SpriteComponent::getFriendlyName() const
    {
        if (pTexture)
            return Utils::getFilenameWithoutExtension(pTexture->getFilename());
        return "";
    }

    void SpriteComponent::draw()
    {
        glm::vec4 col(
            color.r * color.a,
            color.g * color.a,
            color.b * color.a,
            color.a * (1.0f - additive)
        );

        getSpriteBatch()->drawSprite(pTexture,
                                     m_pEntity->getWorldTransformWithScale(),
                                     col, 
                                     glm::vec2(SPRITE_BASE_SCALE),
                                     origin,
                                     uvs);
    }
}
