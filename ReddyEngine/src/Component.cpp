#include "Engine/Component.h"
#include "Engine/Log.h"

namespace Engine
{
	Component::Component()
	{

	}

	Component::~Component()
	{

	}

	void Component::onCreate()
	{
		isEnabled = true;
	}

	void Component::onEnable()
	{
		isEnabled = true;
	}

	void Component::enable()
	{
		isEnabled = true;
	}

	void Component::disable()
	{
		isEnabled = false;
	}

	Json::Value Component::serialize()
	{
		return Json::nullValue;
	}

	void Component::deserialize(Json::Value json)
	{
		CORE_WARN("Deserialize not implemented.");
	}

}

