#pragma once

#include "Engine/Entity.h"

#include <memory>
#include <vector>

namespace Engine
{
	// probably when moving from ReddyGame to Json's files 
	// we should change EntityManager to be only owner of Entities
	using EntityUnique = std::unique_ptr<Entity>;
	using EntityShared = std::shared_ptr<Entity>;

	class EntityManager final
	{
	public:
		EntityManager();
		~EntityManager();

		EntityShared& createEntity();
		EntityShared& createEntity(EntityShared parent);
		void destroyEntity(EntityShared entity);
	private:
		EntityShared root;

		void update(float deltaTime);
		void fixedUpdate(float deltaTime);
	};
}