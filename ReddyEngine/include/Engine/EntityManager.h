#pragma once

#include <json/json.h>

#include <memory>
#include <vector>


namespace Engine
{
	class ComponentManager;
	using ComponentManagerRef = std::shared_ptr<ComponentManager>;


	// Forward declarations
	class Entity;

	using EntityRef = std::shared_ptr<Entity>;

	class EntityManager final
	{
	public:
		EntityManager();
		~EntityManager();

		void clear();

		EntityRef createEntity(); // Adds to root
		EntityRef createEntity(const EntityRef& pParent);
		EntityRef createEntityFromJson(const EntityRef& pParent, const Json::Value& json);
		void destroyEntity(EntityRef pEntity);

		const EntityRef& getRoot() const { return m_pRoot; }

	    Json::Value serialize();
        void deserialize(const Json::Value& json);

		void update(float dt);
		void fixedUpdate(float dt);

	public:
		// Engine use only
		const ComponentManagerRef& getComponentManager() const;

	private:
		EntityRef m_pRoot;
		uint64_t m_id;
		ComponentManagerRef m_pComponentManager;
		std::vector<EntityRef> m_entitiesToDestroy;
	};
}
