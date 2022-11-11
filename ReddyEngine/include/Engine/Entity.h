#pragma once

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
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

		bool operator==(const Transform& other) const
		{
			return 
				position == other.position && 
				rotation == other.rotation && 
				scale == other.scale;
		}

		bool operator!=(const Transform& other) const
		{
			return 
				position != other.position ||
				rotation != other.rotation || 
				scale != other.scale;
		}
	};


	class Entity final : public std::enable_shared_from_this<Entity>
	{
	public:
		uint64_t id = 0;
		std::string name;
		bool sortChildren = false;
		bool mouseChildren = true;
		bool clickThrough = false;
		bool uiRoot = false;

	public:
		~Entity();

		bool addChild(EntityRef pChild); // True if was added, false if already child
		bool removeChild(const EntityRef& pChild); // True if was removed
		EntityRef getParent() const { return m_pParent ? m_pParent->shared_from_this() : nullptr; }
		const std::vector<EntityRef>& getChildren() const { return m_children; }

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
			
			pComponent->m_pEntity = this;
			m_components.push_back(pComponent);
			componentAdded(pComponent);

			return pComponent;
		}

		template<typename T>
		bool removeComponent() const
		{
			return removeComponent(getComponent<T>());
		}

		bool removeComponent(const ComponentRef& pComponent);

		Json::Value serialize(bool includeChildren = true);
		void deserialize(const Json::Value json, bool includeChildren = true);
		
		friend bool operator==(const Entity& lhs, const Entity& rhs)
		{
			return lhs.id == rhs.id;
		}

		void draw();

		const Transform& getTransform() const { return m_transform; }
		const glm::vec2& getPosition() const { return m_transform.position; }
		glm::vec2 getWorldPosition();
		float getRotation() const { return m_transform.rotation; }
		const glm::vec2& getScale() const { return m_transform.scale; }

		void setTransform(const Transform& transform);
		void setPosition(const glm::vec2& position);
		void setRotation(float degrees);
		void setScale(const glm::vec2& scale);
		void setWorldPosition(const glm::vec2& position);

		const glm::mat4& getWorldTransform();
		const glm::mat4& getWorldTransformWithScale();
		const glm::mat4& getInvWorldTransform();
		const glm::mat4& getInvWorldTransformWithScale();

		EntityRef getMouseHover(const glm::vec2& mousePos, bool ignoreMouseFlags = false);

		void onMouseEnter();
		void onMouseLeave();

	public:
		// Editor stuff (We could #ifdef this in final version?)
		bool edit();
		bool isSelected = false;
		void drawOutline(const glm::vec4& color, float zoomScale);
		Json::Value undoJson; // Another editor hack to keep track of it's serialized before change.

	private:
		void componentAdded(const ComponentRef& pComponent);
		void updateDirtyTransforms();
		void setDirtyTransform();
		bool isMouseHover(const glm::vec2& mousePos) const;

		bool m_transformDirty = true;
		Transform m_transform;
		Entity* m_pParent = nullptr;
		std::vector<EntityRef> m_children;
		std::vector<ComponentRef> m_components;
		glm::mat4 m_worldTransform;
		glm::mat4 m_invWorldTransform;
		glm::mat4 m_worldTransformWithScale;
		glm::mat4 m_invWorldTransformWithScale; // For mouse pick
	};
}