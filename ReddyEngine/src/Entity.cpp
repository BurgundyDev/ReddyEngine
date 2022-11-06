#include "Engine/Entity.h"

#include <functional>

namespace Engine
{
	Entity::Entity(EntityRef& parent) : parent(parent.get()), id(0)
	{

	}

	Entity::Entity() : id(0)
	{
		parent = nullptr;
	}

	Entity::~Entity()
	{
	}

	void Entity::update(float deltatime)
	{

	}

	void Entity::fixedUpdate(float deltatime)
	{

	}

	void Entity::onCreate()
	{
		
	}

	void Entity::onDestroy()
	{

	}

	Json::Value Entity::serialize()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void Entity::deserialize(Json::Value json)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}
}