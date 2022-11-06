#include "Engine/Entity.h"

#include <functional>

namespace Engine
{
	Entity::Entity(EntityRef parent) : parent(parent.get()), id(0)
	{

	}

	Entity::Entity() : id(0)
	{
		parent = nullptr;
	}

	Entity::~Entity()
	{
	}

	void Entity::update(float deltaTime)
	{
		auto childs = children;

		for (auto it = childs.begin(); it != childs.end(); it++)
		{
			(*it)->update(deltaTime);
		}
	}

	void Entity::fixedUpdate(float deltaTime)
	{
		auto childs = children;

		for (auto it = childs.begin(); it != childs.end(); it++)
		{
			(*it)->fixedUpdate(deltaTime);
		}
	}

	void Entity::onCreate()
	{
		
	}

	void Entity::onDestroy()
	{

	}

	const Json::Value Entity::serialize()
	{
		return Json::nullValue;
	}

	void Entity::deserialize(const Json::Value json)
	{
		return;
	}
}