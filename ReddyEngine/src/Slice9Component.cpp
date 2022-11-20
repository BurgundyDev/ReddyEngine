#include "Engine/Slice9Component.h"
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
    Slice9Component::Slice9Component()
    {
        pTexture = getResourceManager()->getTexture("textures/defaultTexture.png");
    }

    Json::Value Slice9Component::serialize()
    {
        auto json = Component::serialize();

        json["texture"] = pTexture ? pTexture->getFilename() : "";
        json["color"] = Utils::serializeJsonValue(color);
        json["origin"] = Utils::serializeJsonValue(origin);
        json["scale"] = Utils::serializeJsonValue(scale);
        json["uvs"] = Utils::serializeJsonValue(uvs);
        json["additive"] = Utils::serializeJsonValue(additive);
        json["padding"] = Utils::serializeJsonValue(padding);
        json["repeatCenter"] = Utils::serializeJsonValue(repeatCenter);

        return json;
    }

    void Slice9Component::deserialize(const Json::Value& json)
    {
        Component::deserialize(json);

        pTexture = getResourceManager()->getTexture(Utils::deserializeString(json["texture"]));

        const float DEFAULT_COLOR[4] = {1, 1, 1, 1};
        Utils::deserializeFloat4(&color.r, json["color"], DEFAULT_COLOR);

        const float DEFAULT_ORIGIN[2] = {0.5f, 0.5f};
        Utils::deserializeFloat2(&origin.x, json["origin"], DEFAULT_ORIGIN);

        const float DEFAULT_SCALE[2] = {1, 1};
        Utils::deserializeFloat2(&scale.x, json["scale"], DEFAULT_SCALE);

        const float DEFAULT_UVS[4] = {0, 0, 1, 1};
        Utils::deserializeFloat4(&uvs.x, json["uvs"], DEFAULT_UVS);

        additive = Utils::deserializeFloat(json["additive"], 0.0f);

        const int DEFAULT_PADDING[4] = {0, 0, 0, 0};
        Utils::deserializeInt4(&padding.x, json["padding"], DEFAULT_PADDING);
        repeatCenter = Utils::deserializeBool(json["repeatCenter"], false);
    }

    bool Slice9Component::edit()
    {
        bool changed = false;

        changed |= GUI::textureProperty("Texture", &pTexture, "", isFrameAnimTexture());
        changed |= GUI::colorProperty("Color", &color);
        changed |= GUI::originProperty("Origin", &origin);
        changed |= GUI::floatSliderProperty("Additive", &additive, 0.0f, 1.0f, "Alpha blend to Additive blend ratio.");
        changed |= GUI::vec2Property("scale", &scale);
        changed |= GUI::paddingProperty("padding", &padding);
        changed |= GUI::boolProperty("repeatCenter", &repeatCenter);

        return changed;
    }

    bool Slice9Component::isMouseHover(const glm::vec2& mousePos) const
    {
        if (!pTexture) return Component::isMouseHover(mousePos);
        
        const auto& invTransform = m_pEntity->getInvWorldTransformWithScale();

        glm::ivec2 textureSize = pTexture ? pTexture->getSize() : glm::ivec2{ 1, 1 };
        glm::vec2 sizef = glm::vec2((float)textureSize.x, (float)textureSize.y) * scale * SPRITE_BASE_SCALE;
        glm::vec2 invOrigin(1.f - origin.x, 1.f - origin.y);

        glm::vec2 localMouse = invTransform * glm::vec4(mousePos, 0, 1);

        return 
            localMouse.x >= -sizef.x * origin.x &&
            localMouse.x <= sizef.x * invOrigin.x &&
            localMouse.y >= -sizef.y * origin.y &&
            localMouse.y <= sizef.y * invOrigin.y;
    }

	void Slice9Component::drawOutline(const glm::vec4& color, float zoomScale)
    {
		const auto& transform = m_pEntity->getWorldTransformWithScale();
		auto sb = getSpriteBatch().get();

        glm::ivec2 textureSize = pTexture ? pTexture->getSize() : glm::ivec2{ 1, 1 };
        glm::vec2 sizef = glm::vec2((float)textureSize.x, (float)textureSize.y) * scale * SPRITE_BASE_SCALE;
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

    TextureRef Slice9Component::getEditorIcon() const
    {
        return pTexture ? pTexture : Component::getEditorIcon();
    }

    bool Slice9Component::isFrameAnimTexture() const
    {
        return pTexture != nullptr && m_pEntity->hasComponent<FrameAnimComponent>();
    }

    std::string Slice9Component::getFriendlyName() const
    {
        if (pTexture)
            return Utils::getFilenameWithoutExtension(pTexture->getFilename());
        return "";
    }

    void Slice9Component::draw()
    {
        if (!pTexture) return;

        glm::vec4 col(
            color.r * color.a,
            color.g * color.a,
            color.b * color.a,
            color.a * (1.0f - additive)
        );

        getSpriteBatch()->drawSlice9Sprite(pTexture, m_pEntity->getWorldTransformWithScale(), col, scale * SPRITE_BASE_SCALE, origin, padding);

        //auto size = pTexture->getSize();
        //auto sizexf = (float)size.x;
        //auto sizeyf = (float)size.y;
        //const glm::vec4 paddingUVs{padding.x / sizexf, padding.y / sizeyf, padding.z / sizexf, padding.w / sizeyf};

        //glm::vec2 midScales(
        //    2.75f,//(((1.0f - paddingUVs.z) - paddingUVs.x) * sizexf * scale.x) / (((1.0f - paddingUVs.z) - paddingUVs.x) * sizexf),
        //    1//(sizeyf - (float)padding.w - (float)padding.y) * scale.y / sizeyf
        //);

        // This is so messy and magic... just roll with it (Especially the middle parts...)

        //if (paddingUVs.x && paddingUVs.y)
        //    getSpriteBatch()->drawSprite(pTexture, m_pEntity->getWorldTransformWithScale(), col, glm::vec2(SPRITE_BASE_SCALE),
        //                                 {(scale.x * (origin.x)) * (1.0f / paddingUVs.x), (scale.y * (origin.y)) * (1.0f / paddingUVs.y)},
        //                                 {0.f, 0.f, paddingUVs.x, paddingUVs.y});
        //if (1.0f - paddingUVs.z > paddingUVs.x && paddingUVs.y)
        //    getSpriteBatch()->drawSprite(pTexture, m_pEntity->getWorldTransformWithScale(), col, glm::vec2(midScales.x, 1.0f) * SPRITE_BASE_SCALE,
        //                                 {(scale.x * (origin.x)) * (1.0f / paddingUVs.x), (scale.y * (origin.y)) * (1.0f / paddingUVs.y)},
        //                                 {paddingUVs.x, 0.f, 1.f - paddingUVs.z, paddingUVs.y});
        //if (paddingUVs.z && paddingUVs.y)
        //    getSpriteBatch()->drawSprite(pTexture, m_pEntity->getWorldTransformWithScale(), col, glm::vec2(SPRITE_BASE_SCALE),
        //                                 {1.0f - (scale.x * (1.0f - origin.x)) * (1.0f / paddingUVs.x), (scale.y * (origin.y)) * (1.0f / paddingUVs.y)},
        //                                 {1.f - paddingUVs.z, 0.f, 1.f, paddingUVs.y});

        //getSpriteBatch()->drawSprite(pTexture,
        //                             m_pEntity->getWorldTransformWithScale(),
        //                             col, 
        //                             scale * SPRITE_BASE_SCALE,
        //                             origin,
        //                             {0.f, paddingUVs.y, paddingUVs.x, 1.f - paddingUVs.w});
        //getSpriteBatch()->drawSprite(pTexture,
        //                             m_pEntity->getWorldTransformWithScale(),
        //                             col, 
        //                             scale * SPRITE_BASE_SCALE,
        //                             origin,
        //                             {paddingUVs.x, paddingUVs.y, 1.f - paddingUVs.z, 1.f - paddingUVs.w});
        //getSpriteBatch()->drawSprite(pTexture,
        //                             m_pEntity->getWorldTransformWithScale(),
        //                             col, 
        //                             scale * SPRITE_BASE_SCALE,
        //                             origin,
        //                             {1.f - paddingUVs.z, paddingUVs.y, 1.f, 1.f - paddingUVs.w});

        //if (paddingUVs.x && paddingUVs.w)
        //    getSpriteBatch()->drawSprite(pTexture, m_pEntity->getWorldTransformWithScale(), col, glm::vec2(SPRITE_BASE_SCALE),
        //                                 {(scale.x * (origin.x)) * (1.0f / paddingUVs.z), 1.0f - (scale.y * (1.0f - origin.y)) * (1.0f / paddingUVs.w)},
        //                                 {0.f, 1.f - paddingUVs.w, paddingUVs.x, 1.f});
        //getSpriteBatch()->drawSprite(pTexture,
        //                             m_pEntity->getWorldTransformWithScale(),
        //                             col, 
        //                             scale * SPRITE_BASE_SCALE,
        //                             origin,
        //                             {paddingUVs.x, 1.f - paddingUVs.w, 1.f - paddingUVs.z, 1.f});
        //if (paddingUVs.z && paddingUVs.w)
        //    getSpriteBatch()->drawSprite(pTexture, m_pEntity->getWorldTransformWithScale(), col, glm::vec2(SPRITE_BASE_SCALE),
        //                                 {1.0f - (scale.x * (1.0f - origin.x)) * (1.0f / paddingUVs.z), 1.0f - (scale.y * (1.0f - origin.y)) * (1.0f / paddingUVs.w)},
        //                                 {1.f - paddingUVs.z, 1.f - paddingUVs.w, 1.f, 1.f});
    }
}
