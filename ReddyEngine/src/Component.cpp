#include "Engine/Component.h"
#include "Engine/Log.h"
#include "Engine/Entity.h"


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
		void Component::onCreate()
		{
			m_isEnabled = true;
		}

		void Component::onEnable()
		{
			m_isEnabled = true;
		}

		void Component::enable()
		{
			m_isEnabled = true;
		}

		void Component::disable()
		{
			m_isEnabled = false;
		}
	// END TODO
}

