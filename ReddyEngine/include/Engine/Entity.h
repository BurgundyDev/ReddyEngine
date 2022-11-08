#pragma once

#include <glm/vec2.hpp>
#include <json/json.h>

#include <string>
#include <memory>
#include <vector>


namespace Engine
{
	class Component;
	using ComponentRef = std::shared_ptr<Component>;

	class Entity;
	using EntityRef = std::shared_ptr<Entity>;


	struct Transform 
	{
		glm::vec2 position = glm::vec2(0);
		float rotation = 0;
		glm::vec2 scale = glm::vec2(1);
	};


	class Entity final : public std::enable_shared_from_this<Entity>
	{
	public:
		uint64_t id = 0;
		std::string name;

	public:
		~Entity();

		bool addChild(EntityRef pChild); // True if was added, false if already child
		bool removeChild(const EntityRef& pChild); // True if was removed
		EntityRef getParent() const { return m_pParent ? m_pParent->shared_from_this() : nullptr; }
		const std::vector<EntityRef>& getChildren() const { return m_children; }

		const Transform& getTransform() const { return m_transform; }
		void setTransform(const Transform transform);

		const std::vector<ComponentRef>& getComponents() const { return m_components; }
		
		template<typename T>
		bool hasComponent() const
		{
			for (const auto& pComponent : m_components)
			{
				auto pRet = std::dynamic_pointer_cast<T>(pComponent);
				if (pRet) return true;
			}
			return false;
		}

		template<typename T>
		std::shared_ptr<T> getComponent() const
		{
			for (const auto& pComponent : m_components)
			{
				auto pRet = std::dynamic_pointer_cast<T>(pComponent);
				if (pRet) return pRet;
			}
			return nullptr;
		}

		template<typename T>
		std::shared_ptr<T> addComponent()
		{
			auto pComponent = getComponent<T>();
			if (pComponent) return pComponent;

			pComponent = std::make_shared<T>();
			
			pComponent->entity = this;
			m_components->push_back(pComponent);
			componentAdded(pComponent);

			return pComponent;
		}

		template<typename T>
		bool removeComponent() const
		{
			return removeComponent(getComponent<T>());
		}

		bool removeComponent(const ComponentRef& pComponent);

		Json::Value serialize();
		void deserialize(const Json::Value json);
		
		friend bool operator==(const Entity& lhs, const Entity& rhs)
		{
			return lhs.id == rhs.id;
		}

	public:
		void edit(); // For editor

	private:
		void componentAdded(const ComponentRef& pComponent);

		bool m_transformDirty = true;
		Transform m_transform;
		Entity* m_pParent = nullptr;
		std::vector<EntityRef> m_children;
		std::vector<ComponentRef> m_components;
	};
}