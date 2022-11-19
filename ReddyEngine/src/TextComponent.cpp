#include "Engine/TextComponent.h"
#include "Engine/Utils.h"
#include "Engine/Font.h"
#include "Engine/ResourceManager.h"
#include "Engine/ReddyEngine.h"
#include "Engine/Constants.h"
#include "Engine/GUI.h"
#include "Engine/Entity.h"
#include "Engine/SpriteBatch.h"


namespace Engine
{
    TextComponent::TextComponent()
    {
        pFont = getResourceManager()->getFont("fonts/defaultFont24.json");
    }

    Json::Value TextComponent::serialize()
    {
        auto json = Component::serialize();

        json["font"] = pFont ? pFont->getFilename() : "";
        json["color"] = Utils::serializeJsonValue(color);
        json["origin"] = Utils::serializeJsonValue(origin);
        json["scale"] = Utils::serializeJsonValue(scale);
        json["text"] = Utils::serializeJsonValue(text);

        return json;
    }

    void TextComponent::deserialize(const Json::Value& json)
    {
        Component::deserialize(json);

        pFont = getResourceManager()->getFont(Utils::deserializeString(json["font"], "fonts/defaultFont24.json"));

        const float DEFAULT_COLOR[4] = {1, 1, 1, 1};
        Utils::deserializeFloat4(&color.r, json["color"], DEFAULT_COLOR);

        const float DEFAULT_ORIGIN[2] = {0.5f, 0.5f};
        Utils::deserializeFloat2(&origin.x, json["origin"], DEFAULT_ORIGIN);

        const float DEFAULT_SCALE = 1.0f;
        scale = Utils::deserializeFloat(json["scale"], DEFAULT_SCALE);

        text = Utils::deserializeString(json["text"], "Text");
    }

    bool TextComponent::edit()
    {
        bool changed = false;

        changed |= GUI::multilineStringProperty("Text", &text);
        changed |= GUI::fontProperty("Font", &pFont);
        changed |= GUI::colorProperty("Color", &color);
        changed |= GUI::originProperty("Origin", &origin);
        changed |= GUI::floatProperty("Scale", &scale);

        return changed;
    }

    bool TextComponent::isMouseHover(const glm::vec2& mousePos) const
    {
        if (!pFont || text.empty()) return Component::isMouseHover(mousePos);
        
        const auto& invTransform = m_pEntity->getInvWorldTransformWithScale();

        glm::ivec2 textSize = pFont->measure(text);
        glm::vec2 sizef = glm::vec2((float)textSize.x, (float)textSize.y)/* * m_pEntity->getTransform().scale*/ * scale * SPRITE_BASE_SCALE;
        glm::vec2 invOrigin(1.f - origin.x, 1.f - origin.y);

        glm::vec2 localMouse = invTransform * glm::vec4(mousePos, 0, 1);

        return 
            localMouse.x >= -sizef.x * origin.x &&
            localMouse.x <= sizef.x * invOrigin.x &&
            localMouse.y >= -sizef.y * origin.y &&
            localMouse.y <= sizef.y * invOrigin.y;
    }

    std::string TextComponent::getFriendlyName() const
    {
        auto ret = text.substr(0, 32);
        for (auto& c : ret)
            if (c == '\n')
                c = ' ';
        if (text.size() > 32)
            ret += "...";
        return ret;
    }

	void TextComponent::drawOutline(const glm::vec4& color, float zoomScale)
    {
        if (!pFont || text.empty())
        {
            Component::drawOutline(color, zoomScale);
            return;
        }

		const auto& transform = m_pEntity->getWorldTransformWithScale();
		auto sb = getSpriteBatch().get();

        glm::ivec2 textSize = pFont->measure(text);
        glm::vec2 sizef = glm::vec2((float)textSize.x, (float)textSize.y)/* * m_pEntity->getTransform().scale*/ * scale * SPRITE_BASE_SCALE;
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

    void TextComponent::draw()
    {
        if (!pFont || text.empty()) {
            Component::draw();

            return;
        }

        pFont->draw(text, 
                    m_pEntity->getWorldPosition(),
                    color,
                    m_pEntity->getRotation(),
                    m_pEntity->getTransform().scale.x * scale * SPRITE_BASE_SCALE,
                    origin);
        //getSpriteBatch()->drawSprite(pTexture,
        //                             m_pEntity->getWorldTransformWithScale(),
        //                             color, 
        //                             m_pEntity->getTransform().scale * SPRITE_BASE_SCALE,
        //                             origin,
        //                             uvs);
    }
}
