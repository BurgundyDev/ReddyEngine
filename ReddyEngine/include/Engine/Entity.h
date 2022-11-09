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

	class Entity final
	{
	private:
		Transform m_transform;

	public:
		Entity(const EntityRef& parent);
		Entity();
		~Entity();

		uint64_t						id;
		Entity*							parent;
		std::vector<EntityRef>			children;
		std::vector<ComponentRef>		components;

		void update(float deltaTime);
		void fixedUpdate(float deltaTime);
		void onCreate();
		void onDestroy();
		
		template<typename T>
		bool hasComponent() const
		{
			for (auto it = components.begin(); it != components.end(); it++)
			{
				if (dynamic_cast<T*>(it->get())) return true;
			}
		}

		template<typename T>
		const T& addComponent()
		{
			audo pComponent = getComponent<T>();
			if (pComponent) return component;

			pComponent = std::shared_ptr<T>(new T());
			
			pComponent->entity = this;
			components->push_back(pComponent);
			*pComponent->onCreate();

			return pComponent;
		}

		template<typename T>
		const T& getComponent() const
		{
			for (auto& component : compnents)
			{
				if (dynamic_cast<T*>(component.get()))
					return dynamic_cast<T>(component);
			}
			return nullptr;
		}

		const Transform getTransform()
		{
			return m_transform;
		}

		void setTransform(const Transform transform)
		{
			m_transform = transform;
		}

		const Json::Value serialize();
		void deserialize(const Json::Value json);
		
		friend bool operator==(const Entity& lhs, const Entity& rhs)
		{
			return lhs.id == rhs.id;
		}

	public:
		// Editor stuff (We could #ifdef this in final version?)
		bool edit();
		bool isSelected = false;

	private:
		void componentAdded(const ComponentRef& pComponent);

		bool m_transformDirty = true;
		Transform m_transform;
		Entity* m_pParent = nullptr;
		std::vector<EntityRef> m_children;
		std::vector<ComponentRef> m_components;
	};
}