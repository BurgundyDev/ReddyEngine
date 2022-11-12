#include "Engine/Component.h"
#include "Engine/Log.h"
#include "Engine/Entity.h"
#include "Engine/Utils.h"
#include "Engine/Scene.h"
#include "Engine/ReddyEngine.h"
#include "Engine/SpriteBatch.h"
#include "Engine/SpriteComponent.h"
#include "Engine/TextComponent.h"


namespace Engine
{
	ComponentRef Component::create(const std::string& className)
	{
		if (className == "Sprite") return std::make_shared<SpriteComponent>();
		if (className == "Text") return std::make_shared<TextComponent>();
		CORE_ERROR("Unrecognized Component Type: {}", className);
		return nullptr;
	}

	Component::Component()
	{
	}

	Component::~Component()
	{
	}
	
	EntityRef Component::getEntity()
	{
		return m_pEntity ? m_pEntity->shared_from_this() : nullptr;
	}

	void Component::enable()
	{
		if (!m_isEnabled)
		{
			m_isEnabled = true;
			onEnable();
		}
	}

	void Component::disable()
	{
		if (m_isEnabled)
		{
			m_isEnabled = false;
			onDisable();
		}
	}

	Json::Value Component::serialize()
	{
		Json::Value json;
		json["type"] = getType();
		json["enabled"] = m_isEnabled;
		return json;
	}

	void Component::deserialize(const Json::Value& json)
	{
		m_isEnabled = Utils::deserializeBool(json["enabled"], true);
	}

	bool Component::isMouseHover(const glm::vec2& mousePos) const
	{
		if (!getScene()->isEditorScene()) return false;

		auto worldPos = m_pEntity->getWorldPosition();
		return 
			mousePos.x >= worldPos.x - 0.2f &&
			mousePos.x <= worldPos.x + 0.2f &&
			mousePos.y >= worldPos.y - 0.2f &&
			mousePos.y <= worldPos.y + 0.2f;
	}

	void Component::drawOutline(const glm::vec4& color, float zoomScale)
	{
		auto worldPos = m_pEntity->getWorldPosition();
		auto sb = getSpriteBatch().get();

		sb->drawLine({worldPos.x - 0.2f, worldPos.y - 0.2f}, {worldPos.x - 0.2f, worldPos.y + 0.2f}, 2.0f * zoomScale, color);
		sb->drawLine({worldPos.x - 0.2f, worldPos.y + 0.2f}, {worldPos.x + 0.2f, worldPos.y + 0.2f}, 2.0f * zoomScale, color);
		sb->drawLine({worldPos.x + 0.2f, worldPos.y + 0.2f}, {worldPos.x + 0.2f, worldPos.y - 0.2f}, 2.0f * zoomScale, color);
		sb->drawLine({worldPos.x + 0.2f, worldPos.y - 0.2f}, {worldPos.x - 0.2f, worldPos.y - 0.2f}, 2.0f * zoomScale, color);
	}
}

