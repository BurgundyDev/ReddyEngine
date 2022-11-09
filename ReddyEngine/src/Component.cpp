#include "Engine/Component.h"
#include "Engine/Log.h"
#include "Engine/Entity.h"
#include "Engine/Utils.h"
#include "Engine/SpriteComponent.h"


namespace Engine
{
	ComponentRef Component::create(const std::string& className)
	{
		if (className == "Sprite") return std::make_shared<SpriteComponent>();
		CORE_ERROR("Unrecognized Component Type: " + className);
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
}

