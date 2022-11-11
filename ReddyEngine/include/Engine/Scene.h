#pragma once

#include <glm/vec2.hpp>
#include <json/json.h>

#include <memory>
#include <vector>


namespace Engine
{
	class ComponentManager;
	using ComponentManagerRef = std::shared_ptr<ComponentManager>;

	class Entity;
	using EntityRef = std::shared_ptr<Entity>;

	struct EntitySearchParams;

	class Scene final
	{
	public:
		Scene();
		~Scene();

		bool isEditorScene() const { return m_isEditorScene; }
		void setEditorScene(bool editorScene) { m_isEditorScene = editorScene; }

		void clear();

		EntityRef createEntity(); // Adds to root
		EntityRef createEntity(const EntityRef& pParent);
		EntityRef createEntityFromJson(const EntityRef& pParent, const Json::Value& json);
		void destroyEntity(EntityRef pEntity);
		void destroyEntity(uint64_t id);

		EntityRef findEntity(const EntityRef& pEntity, uint64_t id);
		EntityRef findEntity(uint64_t id);

		/*! \brief Search the root for an entity with the given name */
		EntityRef getEntityByName(const std::string& name, bool recursive = false) const;
		/*! \brief Search the root for an entity with the given name, optionally searching  using EntitySearchParams */
		EntityRef getEntityByName(const std::string& name, const EntitySearchParams& searchParams, bool recursive = false) const;
		
		const EntityRef& getRoot() const { return m_pRoot; }

	    Json::Value serialize();
        void deserialize(const Json::Value& json);

		void update(float dt);
		void fixedUpdate(float dt);
		void draw();

		const glm::vec2& getMousePos() const { return m_mousePos; }
		void setMousePos(const glm::vec2& mousePos) { m_mousePos = mousePos; } // In World coordinates

		const EntityRef& getHoveredEntity() const { return m_pMouseHoverEntity; }

	public:
		// Engine use only
		const ComponentManagerRef& getComponentManager() const;

	private:
		bool m_isEditorScene = false;
		glm::vec2 m_mousePos; // In World coordinates
		EntityRef m_pRoot;
		EntityRef m_pMouseHoverEntity;
		uint64_t m_id;
		ComponentManagerRef m_pComponentManager;
		std::vector<EntityRef> m_entitiesToDestroy;
	};
}
