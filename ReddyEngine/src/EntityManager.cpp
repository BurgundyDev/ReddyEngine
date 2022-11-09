#include "Engine/EntityManager.h"
#include "Engine/Entity.h"


#include <algorithm>

namespace Engine
{

	EntityManager::EntityManager() : m_pRoot(new Entity()), m_id(0)
	{
		m_pRoot->name = "Root";
	}

	EntityManager::~EntityManager()
	{
		delete m_pRoot;
	}

	const EntityRef EntityManager::createEntity()
	{
		EntityRef root = std::make_shared<Entity>(*m_pRoot);
		return createEntity(root);
	}

	const EntityRef EntityManager::createEntity(const EntityRef& parent)
	{
		EntityRef newEntity = std::make_shared<Entity>(Entity(parent));
		newEntity->parent = parent.get();
		newEntity->id = ++m_id;

		parent->children.push_back(newEntity);
		
		newEntity->onCreate();
		return newEntity;
	}

	void EntityManager::destroyEntity(const EntityRef& entity)
	{
		entity->onDestroy();

		{
			auto parent = entity->parent;
			parent->children.erase(std::remove(parent->children.begin(), parent->children.end(), entity));
		}
	}
	void EntityManager::update(float deltaTime)
	{
		m_pRoot->update(deltaTime);
	}

	void EntityManager::fixedUpdate(float deltaTime)
	{
		m_pRoot->fixedUpdate(deltaTime);

		auto childs = m_pRoot->children;

		for (auto it = childs.begin(); it != childs.end(); it++)
		{
			(*it)->fixedUpdate(deltaTime);
		}
	}

}