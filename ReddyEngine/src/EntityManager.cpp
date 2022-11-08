#include "Engine/EntityManager.h"
#include "Engine/Entity.h"
#include "Engine/Log.h"
#include "ComponentManager.h"

#include <algorithm>

namespace Engine
{

	EntityManager::EntityManager()
		: m_pRoot(std::make_shared<Entity>())
		, m_id(0)
		, m_pComponentManager(std::make_shared<ComponentManager>())
	{
	}

	EntityManager::~EntityManager()
	{
	}

	Json::Value EntityManager::serialize()
	{
		return m_pRoot->serialize();
	}

    void EntityManager::deserialize(const Json::Value& json)
	{
		clear();
		m_pRoot->deserialize(json);
	}

	void EntityManager::clear()
	{
		m_pRoot.reset();
		m_pRoot = std::make_shared<Entity>();
	}

	EntityRef EntityManager::createEntity()
	{
		EntityRef pNewEntity = std::make_shared<Entity>();
		pNewEntity->id = ++m_id;
		m_pRoot->addChild(pNewEntity);
		return pNewEntity;
	}

	EntityRef EntityManager::createEntity(const EntityRef& pParent)
	{
		EntityRef pNewEntity = std::make_shared<Entity>();
		pNewEntity->id = ++m_id;
		pParent->addChild(pNewEntity);
		return pNewEntity;
	}

	EntityRef EntityManager::createEntityFromJson(const EntityRef& pParent, const Json::Value& json)
	{
		EntityRef pNewEntity = std::make_shared<Entity>();
		pNewEntity->deserialize(json);
		pParent->addChild(pNewEntity);
		m_id = std::max(m_id, pNewEntity->id + 1); // If we're loading a file, make sure next id is at least higher than this one
		return pNewEntity;
	}

	const ComponentManagerRef& EntityManager::getComponentManager() const
	{
		return m_pComponentManager;
	}

	void EntityManager::destroyEntity(EntityRef pEntity)
	{
		if (pEntity == m_pRoot)
			CORE_FATAL(false, "Cannot erase Root entity!");

		if (pEntity->getParent())
			pEntity->getParent()->removeChild(pEntity);

		const auto& components = pEntity->getComponents();
		for (const auto& pComponent : components)
			m_pComponentManager->removeComponent(pComponent);

		m_entitiesToDestroy.push_back(pEntity); // We keep that around because components' onDestroy() might still refer to the entity
	}

	void EntityManager::update(float dt)
	{
		m_entitiesToDestroy.clear();

		m_pComponentManager->update(dt);

		m_entitiesToDestroy.clear();
	}

	void EntityManager::fixedUpdate(float dt)
	{
		m_entitiesToDestroy.clear();

		m_pComponentManager->fixedUpdate(dt);

		m_entitiesToDestroy.clear();
	}
}
