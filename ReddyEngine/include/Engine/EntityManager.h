#pragma once

#include <memory>
#include <vector>

namespace Engine
{
	// Forward declarations
	class Entity;

	using EntityRef = std::shared_ptr<Entity>;

	class EntityManager final
	{
	public:
		EntityManager();
		~EntityManager();

		void update(float deltaTime);
		void fixedUpdate(float deltaTime);

		const EntityRef createEntity();
		const EntityRef createEntity(const EntityRef& parent);
		void destroyEntity(const EntityRef& entity);
	
	private:
		Entity* m_pRoot;
		uint64_t m_id;
	};
}