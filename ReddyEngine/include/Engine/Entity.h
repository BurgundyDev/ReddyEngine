#pragma once

#include "Engine/Component.h"

#include <glm/vec2.hpp>

#include <string>
#include <memory>
#include <vector>

struct Transform 
{
	glm::vec2 position = glm::vec2(0);
	float rotation = 0;
	glm::vec2 scale = glm::vec2(0);
};

namespace Engine
{
	using EntityRef = std::shared_ptr<Entity>;
	using ComponentRef = std::shared_ptr<Component>;
	using TransformRef = std::shared_ptr<Transform>;

	class Entity
	{
	private:
		Transform m_transform;

	public:
		Entity(EntityRef& parent);
		Entity();
		~Entity();

		uint64_t						id;
		Entity*							parent;
		std::vector<EntityRef>			children;
		std::vector<ComponentRef>		components;

		void update(float deltatime);
		void fixedUpdate(float deltatime);
		void onCreate();
		void onDestroy();
		
		template<typename T>
		bool hasComponent() const
		{
			for (auto it = components.begin(); it != components.end(); it++)
			{
				if (it->is_class<T>()) return true;
			}
		}

		template<typename T, typename... TArgs>
		const T& addComponent(TArgs&&... mArgs)
		{
			T* c(new T(std::forward<TArgs>(mArgs)...));
			c->entity = this;
			std::unique_ptr<Component> compPtr(c);
			components->push_back(compPtr);

			*compPtr->onCreate();
		}

		template<typename T>
		const T& getComponent() const
		{
			T* c(new T(std::forward<TArgs>(mArgs)...));
			c->entity = this;
			std::unique_ptr<Component> uPtr(c);
			components->push_back(c);
		}

		Transform getTransform()
		{
			return m_transform;
		}

		void setTransform(Transform transform)
		{
			m_transform = transform;
		}

		Json::Value serialize();
		void deserialize(Json::Value json);
		
		friend bool operator==(const Entity& lhs, const Entity& rhs)
		{
			return lhs.id == rhs.id;
		}
	};
}