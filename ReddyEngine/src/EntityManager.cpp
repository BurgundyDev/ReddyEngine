#include "Engine/EntityManager.h"

#include <algorithm>

namespace Engine
{

	EntityManager::EntityManager() : m_root(new Entity())
	{
		
	}

	EntityManager::~EntityManager()
	{
		delete m_root;
	}

	EntityRef& EntityManager::createEntity()
	{
		return createEntity(m_root);
	}

	EntityRef& EntityManager::createEntity(Entity* parent)
	{
		EntityRef parentRef = std::make_shared<Entity>(parent);
		return createEntity(parentRef);
	}

	EntityRef& EntityManager::createEntity(EntityRef parent)
	{
		EntityRef newEntity = std::make_shared<Entity>(parent);
		newEntity->id = ++m_id;

		parent->children.push_back(newEntity);
		
		newEntity->onCreate();
		return newEntity;
	}

	void EntityManager::destroyEntity(EntityRef entity)
	{
		entity->onDestroy();

		{
			auto parent = entity->parent;
			parent->children.erase(std::remove(parent->children.begin(), parent->children.end(), entity));
		}
  
		entity.reset();
	}
	void EntityManager::update(float deltaTime)
	{
		m_root->update(deltaTime);

		auto childs = m_root->children;

		for (auto it = childs.begin(); it != childs.end(); it++)
		{
			(*it)->update(deltaTime);
		}
	}

	void EntityManager::fixedUpdate(float deltaTime)
	{
		m_root->fixedUpdate(deltaTime);

		auto childs = m_root->children;

		for (auto it = childs.begin(); it != childs.end(); it++)
		{
			(*it)->fixedUpdate(deltaTime);
		}
	}

}