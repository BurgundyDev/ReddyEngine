#include "Engine/Component.h"
#include "Engine/Log.h"
#include "Engine/Entity.h"
#include "Engine/Utils.h"


namespace Engine
{
	ComponentRef Component::create(const std::string& className)
	{
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

	// TODO
		void Component::enable()
		{
			m_isEnabled = true;
		}

		void Component::disable()
		{
			m_isEnabled = false;
		}
	// END TODO

	Json::Value Component::serialize()
	{
		Json::Value json;
		json["type"] = getType();
		json["enabled"] = m_isEnabled;
		return json;
	}

	void Component::deserialize(Json::Value json)
	{
		m_isEnabled = Utils::deserializeBool(json["enabled"], true);
	}
}

