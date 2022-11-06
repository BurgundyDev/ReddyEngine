#include "Engine/Component.h"

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
		throw std::logic_error("The method or operation is not implemented.");
	}

	void Component::deserialize(Json::Value json)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

}

