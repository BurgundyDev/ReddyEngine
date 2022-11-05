#include "Engine/EntityManager.h"

#include <algorithm>

namespace Engine
{

	EntityManager::EntityManager() : root(std::make_shared<Entity>())
	{
		
	}

	EntityManager::~EntityManager()
	{
		root.reset();
	}

	EntityShared& EntityManager::createEntity()
	{
		return createEntity(root);
	}

	EntityShared& EntityManager::createEntity(EntityShared parent)
	{
		EntityShared newEntity = std::make_shared<Entity>(parent);

		parent->children.push_back(newEntity);
		
		newEntity->onCreate();
		return newEntity;
	}

	void EntityManager::destroyEntity(EntityShared entity)
	{
		entity->onDestroy();

		{
			auto parent = entity->parent;
			std::remove(parent->children.begin(), parent->children.end(), entity);
		}
  
		entity.reset();
	}
	void EntityManager::update(float deltaTime)
	{
		root->update(deltaTime);

		auto childs = root->children;

		for (auto it = childs.begin(); it != childs.end(); it++)
		{
			(*it)->update(deltaTime);
		}
	}

	void EntityManager::fixedUpdate(float deltaTime)
	{
		root->fixedUpdate(deltaTime);

		auto childs = root->children;

		for (auto it = childs.begin(); it != childs.end(); it++)
		{
			(*it)->fixedUpdate(deltaTime);
		}
	}

}