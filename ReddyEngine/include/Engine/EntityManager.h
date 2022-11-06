#pragma once

#include "Engine/Entity.h"

#include <memory>
#include <vector>

namespace Engine
{
	using EntityRef = std::shared_ptr<Entity>;

	class EntityManager final
	{
	public:
		EntityManager();
		~EntityManager();

		void update(float deltaTime);
		void fixedUpdate(float deltaTime);

		EntityRef& createEntity();
		EntityRef& createEntity(Entity* parent);
		EntityRef& createEntity(EntityRef parent);
		void destroyEntity(EntityRef entity);
	
	private:
		Entity* m_root;
		uint64_t m_id;
	};
}