#include "Engine/Scene.h"
#include "Engine/Entity.h"
#include "Engine/Log.h"
#include "ComponentManager.h"

#include <algorithm>

namespace Engine
{
	Scene::Scene()
		: m_pRoot(std::make_shared<Entity>())
		, m_id(0)
		, m_pComponentManager(std::make_shared<ComponentManager>())
	{
		m_pRoot->name = "Root";
		m_pRoot->clickThrough = true; // We cannot select the root
	}

	Scene::~Scene()
	{
	}

	Json::Value Scene::serialize()
	{
		Json::Value json;
		json["root"] = m_pRoot->serialize();
		return json;
	}

    void Scene::deserialize(const Json::Value& json)
	{
		clear();
		m_pRoot->deserialize(json["root"]);
		m_pRoot->clickThrough = true; // We cannot select the root
	}

	void Scene::clear()
	{
		m_pRoot.reset();
		m_pRoot = std::make_shared<Entity>();
		m_pRoot->name = "Root";
		m_pRoot->clickThrough = true; // We cannot select the root
	}

	EntityRef Scene::createEntity()
	{
		EntityRef pNewEntity = std::make_shared<Entity>();
		pNewEntity->id = ++m_id;
		m_pRoot->addChild(pNewEntity);
		return pNewEntity;
	}

	EntityRef Scene::createEntity(const EntityRef& pParent)
	{
		EntityRef pNewEntity = std::make_shared<Entity>();
		pNewEntity->id = ++m_id;
		pParent->addChild(pNewEntity);
		return pNewEntity;
	}

	EntityRef Scene::createEntityFromJson(const EntityRef& pParent, const Json::Value& json)
	{
		EntityRef pNewEntity = std::make_shared<Entity>();
		pNewEntity->deserialize(json);
		pParent->addChild(pNewEntity);
		m_id = std::max(m_id, pNewEntity->id + 1); // If we're loading a file, make sure next id is at least higher than this one
		return pNewEntity;
	}

	const ComponentManagerRef& Scene::getComponentManager() const
	{
		return m_pComponentManager;
	}

	void Scene::destroyEntity(EntityRef pEntity)
	{
		if (pEntity == m_pRoot)
			CORE_FATAL("Cannot erase Root entity!");

		if (pEntity->getParent())
			pEntity->getParent()->removeChild(pEntity);

		const auto& components = pEntity->getComponents();
		for (const auto& pComponent : components)
			m_pComponentManager->removeComponent(pComponent);

		m_entitiesToDestroy.push_back(pEntity); // We keep that around because components' onDestroy() might still refer to the entity
	}

	EntityRef Scene::getEntityByName(const std::string& name, bool recursive) const
	{
		return m_pRoot->getChildByName(name, recursive);
	}

	EntityRef Scene::getEntityByName(const std::string& name, const EntitySearchParams& searchParams, bool recursive) const
	{
		return m_pRoot->getChildByName(name, searchParams, recursive);
    }
    
	EntityRef Scene::findEntity(const EntityRef& pEntity, uint64_t id)
	{
		if (pEntity->id == id) return pEntity;
		for (const auto& pChild : pEntity->getChildren())
		{
			auto pRet = findEntity(pChild, id);
			if (pRet) return pRet;
		}
		return nullptr;
	}

	void Scene::destroyEntity(uint64_t id)
	{
		auto pEntity = findEntity(m_pRoot, id);
		if (!pEntity) return;
		destroyEntity(pEntity);
	}

	void Scene::update(float dt)
	{
		m_pComponentManager->update(dt);
		m_entitiesToDestroy.clear();

		// Get the current mouse hover entity (Used by editor, but also gameplay when clicking stuff in UI, or in the world)
		auto pPreviousHoverEntity = m_pMouseHoverEntity;
		m_pMouseHoverEntity = m_pRoot->getMouseHover(m_mousePos, isEditorScene());
		if (m_pMouseHoverEntity == m_pRoot) m_pMouseHoverEntity = nullptr; // We ignore root (In case we're in editor)
	}

	void Scene::fixedUpdate(float dt)
	{
		m_pComponentManager->fixedUpdate(dt);
		m_entitiesToDestroy.clear();
	}

	void Scene::draw()
	{
		m_pRoot->draw();
	}
}
