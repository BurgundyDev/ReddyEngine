#include "Engine/Scene.h"
#include "Engine/Entity.h"
#include "Engine/Log.h"
#include "Engine/EventSystem.h"
#include "Engine/ReddyEngine.h"
#include "ComponentManager.h"

#include <algorithm>

namespace Engine
{
	Scene::Scene()
	{
	}

	void Scene::init()
	{
		m_pRoot = (std::make_shared<Entity>());
		m_id = 0;
		m_pComponentManager = std::make_shared<ComponentManager>();

		m_pRoot->name = "Root";
		m_pRoot->clickThrough = true; // We cannot select the root

		REGISTER_EVENT(MouseButtonDownEvent, Scene::onMouseDown);
		REGISTER_EVENT(MouseButtonUpEvent, Scene::onMouseUp);
	}

	Scene::~Scene()
	{
		clear();
		DEREGISTER_EVENT(MouseButtonDownEvent);
		DEREGISTER_EVENT(MouseButtonUpEvent);
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
		m_isMouseDown = false;
		m_pMouseDownEntity.reset();
		m_pMouseHoverEntity.reset();
		m_pComponentManager->clear();

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

	EntityRef Scene::createEntityFromJson(const EntityRef& pParent, const Json::Value& json, bool generateNewIds)
	{
		EntityRef pNewEntity = std::make_shared<Entity>();
		pParent->addChild(pNewEntity);
		pNewEntity->deserialize(json, true, generateNewIds);
		if (generateNewIds)
			m_id = ++m_id;
		else
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

	void Scene::FindEntitiesByName(const std::string& name, std::vector<EntityRef>& entities, bool recursive) const
	{
		m_pRoot->findByName(name, entities, recursive);
	}

	EntityRef Scene::getEntityByName(const std::string& name, const EntitySearchParams& searchParams, bool recursive) const
	{
		return m_pRoot->getChildByName(name, searchParams, recursive);
    }

	void Scene::FindEntitiesByName(const std::string& name, std::vector<EntityRef>& entities, const EntitySearchParams& searchParams, bool recursive) const
	{
		m_pRoot->findByName(name, entities, searchParams, recursive);
	}
    
	EntityRef Scene::findEntity(uint64_t id)
	{
		return findEntity(m_pRoot, id);
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

	void Scene::onMouseDown(IEvent* pEvent)
	{
		auto pMouseDown = (MouseButtonDownEvent*)pEvent;
		if (pMouseDown->button.button == SDL_BUTTON_LEFT)
		{
			m_isMouseDown = true;
			if (m_pMouseHoverEntity)
			{
				m_pMouseDownEntity = m_pMouseHoverEntity;
				m_pMouseDownEntity->onMouseDown();
			}
		}
	}

	void Scene::onMouseUp(IEvent* pEvent)
	{
		auto pMouseUp = (MouseButtonUpEvent*)pEvent;
		if (pMouseUp->button.button == SDL_BUTTON_LEFT)
		{
			m_isMouseDown = false;
			if (m_pMouseDownEntity)
			{
				m_pMouseDownEntity->onMouseUp();
				if (m_pMouseDownEntity == m_pMouseHoverEntity)
				{
					m_pMouseDownEntity->onMouseClick();
				}
				else
				{
					if (m_pMouseHoverEntity)
						m_pMouseHoverEntity->onMouseEnter();
				}
				m_pMouseDownEntity = nullptr;
			}
		}
	}

	void Scene::update(float dt)
	{
		m_pComponentManager->update(dt);
		m_entitiesToDestroy.clear();

		// Get the current mouse hover entity (Used by editor, but also gameplay when clicking stuff in UI, or in the world)
		auto pPreviousHoverEntity = m_pMouseHoverEntity;
		m_pMouseHoverEntity = m_pRoot->getMouseHover(m_mousePos, isEditorScene());
		if (m_pMouseHoverEntity == m_pRoot) m_pMouseHoverEntity = nullptr; // We ignore root (In case we're in editor)

		if (!m_isEditorScene)
		{
			if (pPreviousHoverEntity != m_pMouseHoverEntity)
			{
				if (m_pMouseDownEntity)
				{
					if (m_pMouseHoverEntity == m_pMouseDownEntity)
						m_pMouseDownEntity->onMouseEnter();
					else if (pPreviousHoverEntity == m_pMouseDownEntity)
						m_pMouseDownEntity->onMouseLeave();
				}
				else
				{
					if (pPreviousHoverEntity) pPreviousHoverEntity->onMouseLeave();
					if (m_pMouseHoverEntity) m_pMouseHoverEntity->onMouseEnter();
				}
			}
		}
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
