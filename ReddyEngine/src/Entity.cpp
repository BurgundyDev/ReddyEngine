#include "Engine/Entity.h"
#include "Engine/Component.h"
#include "Engine/Utils.h"
#include "Engine/Log.h"
#include "Engine/Scene.h"
#include "Engine/ReddyEngine.h"
#include "Engine/GUI.h"
#include "ComponentManager.h"

#include <imgui.h>
#include <glm/gtx/transform.hpp>

#include <functional>


namespace Engine
{
	Entity::~Entity()
	{
	}

	bool Entity::addChild(EntityRef pChild, int insertAt)
	{
		if (pChild->m_pParent) pChild->m_pParent->removeChild(pChild); // This could potentially make the pChild shared_ptr const reference invalid, that's why we pass by value
		for (const auto& pMyChild : m_children) if (pMyChild == pChild) return false;

		if (insertAt < 0 || insertAt > (int)m_children.size())
			m_children.push_back(pChild);
		else
			m_children.insert(m_children.begin() + insertAt, pChild);

		pChild->m_pParent = this;
		return true;
	}

	bool Entity::removeChild(const EntityRef& pChild)
	{
		auto rpChild = pChild.get();
		for (auto it = m_children.begin(); it != m_children.end(); ++it)
		{
			if (it->get() == rpChild)
			{
				rpChild->m_pParent = nullptr;
				m_children.erase(it);
				return true;
			}
		}
		return false;
	}

	int Entity::getChildIndex(const EntityRef& pChild) const
	{
		for (int i = 0, len = (int)m_children.size(); i < len; ++i)
		{
			if (m_children[i] == pChild)
				return i;
		}
		return -1;
	}

	bool Entity::removeComponent(const ComponentRef& pComponent)
	{
		for (auto it = m_components.begin(); it != m_components.end(); ++it)
		{
			if (*it == pComponent)
			{
				getScene()->getComponentManager()->removeComponent(pComponent);
				m_components.erase(it);
				return true;
			}
		}
		return false;
	}

	void Entity::componentAdded(const ComponentRef& pComponent)
	{
		getScene()->getComponentManager()->addComponent(pComponent);
	}

	EntityRef Entity::getChildByName(const std::string& name, bool recursive)
	{
		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			const EntityRef &ref = *it;

			if (ref->name == name) {
				return ref;
			}

			if (recursive) {
				if (auto found_child = ref->getChildByName(name, true)) {
					return found_child;
				}
			}
		}

		return nullptr;
	}

	EntityRef Entity::getChildByName(const std::string& name, const EntitySearchParams& searchParams, bool recursive)
	{
		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			const EntityRef &ref = *it;

			if (ref->name == name && (searchParams.radius < FLT_EPSILON || ref->isInRadius(searchParams.pointInWorld, searchParams.radius)))
			{
				return ref;
			}

			if (recursive) {
				if (auto found_child = ref->getChildByName(name, searchParams, true)) {
					return found_child;
				}
			}
		}

		return nullptr;
	}

	bool Entity::hasChild(const EntityRef& pChild, bool recursive) const
	{
		for (const auto& pMyChild : m_children)
		{
			if (pChild == pMyChild) return true;
			if (recursive)
			{
				if (pMyChild->hasChild(pChild, recursive)) return true;
			}
		}
		return false;
	}

	Json::Value Entity::serialize(bool includeChildren)
	{
		Json::Value json;

		json["id"] = id;
		json["name"] = name;
		json["sortChildren"] = sortChildren;
		json["mouseChildren"] = mouseChildren;
		json["clickThrough"] = clickThrough;
		json["uiRoot"] = uiRoot;
		json["lockScale"] = lockScale;
		json["transform"]["position"] = Utils::serializeJsonValue(m_transform.position);
		json["transform"]["rotation"] = Utils::serializeJsonValue(m_transform.rotation);
		json["transform"]["scale"] = Utils::serializeJsonValue(m_transform.scale);

		// Components
		Json::Value componentsJson(Json::arrayValue);
		for (const auto& pComponent : m_components)
			componentsJson.append(pComponent->serialize());
		json["components"] = componentsJson;

		// Children
		if (includeChildren)
		{
			Json::Value childrenJson(Json::arrayValue);
			for (const auto& pChild : m_children)
				childrenJson.append(pChild->serialize());
			json["children"] = childrenJson;
		}

		return json;
	}

	void Entity::deserialize(const Json::Value json, bool includeChildren, bool generateNewIds)
	{
		for (const auto& pComponent : m_components)
		{
			getScene()->getComponentManager()->removeComponent(pComponent);
		}
		m_components.clear();

		if (generateNewIds)
			id = getScene()->generateEntityId();
		else
			id = Utils::deserializeUInt64(json["id"]);
		getScene()->updateMaxId(id);
		name = Utils::deserializeString(json["name"]);
		sortChildren = Utils::deserializeBool(json["sortChildren"], false);
		mouseChildren = Utils::deserializeBool(json["mouseChildren"], true);
		clickThrough = Utils::deserializeBool(json["clickThrough"], false);
		uiRoot = Utils::deserializeBool(json["uiRoot"], false);
		lockScale = Utils::deserializeBool(json["lockScale"], true);

		// Transform
		m_transform.position = Utils::deserializeJsonValue<glm::vec2>(json["transform"]["position"]);
		m_transform.rotation = Utils::deserializeFloat(json["transform"]["rotation"], 0.0f);
		const float DEFAULT_SCALE[2] = { 1.0f, 1.0f };
		Utils::deserializeFloat2(&m_transform.scale.x, json["transform"]["scale"], DEFAULT_SCALE);

		// Components
		const auto& componentsJson = json["components"];
		for (const auto& componentJson : componentsJson)
		{
			std::string type = Utils::deserializeString(componentJson["type"]);
			auto pComponent = Component::create(type);
			if (!pComponent)
			{
				continue;
			}

			pComponent->deserialize(componentJson);
			pComponent->m_pEntity = this;
			m_components.push_back(pComponent);
			componentAdded(pComponent);
		}

		// Children
		if (includeChildren)
		{
			m_children.clear();
			const auto& childrenJson = json["children"];
			for (const auto& childJson : childrenJson)
			{
				getScene()->createEntityFromJson(shared_from_this(), childJson, generateNewIds);
			}
			m_transformDirty = true;
		}
		else
		{
			setDirtyTransform();
		}
	}

	glm::vec2 Entity::getWorldPosition()
	{
		const auto& worldTransform = getWorldTransform();
		return worldTransform[3];
	}
	
	void Entity::setWorldPosition(const glm::vec2& position)
	{
		if (!m_pParent)
		{
			// Root
			m_transform.position = position;
			setDirtyTransform();
			return;
		}

		m_transform.position = m_pParent->getInvWorldTransform() * glm::vec4(position, 0, 1);
		setDirtyTransform();
	}
	
	void Entity::setTransform(const Transform& transform)
	{
		m_transform = transform;
		setDirtyTransform();
	}

	void Entity::setPosition(const glm::vec2& position)
	{
		m_transform.position = position;
		setDirtyTransform();
	}

	void Entity::setRotation(float degrees)
	{
		m_transform.rotation = degrees;
		setDirtyTransform();
	}

	void Entity::setScale(const glm::vec2& scale)
	{
		m_transform.scale = scale;
		setDirtyTransform();
	}

	void Entity::setDirtyTransform()
	{
		m_transformDirty = true;
		for (const auto& pChild : m_children)
			pChild->setDirtyTransform();
	}

	void Entity::updateDirtyTransforms()
	{
		if (!m_transformDirty) return;

		glm::mat4 localTransform = 
			glm::translate(glm::vec3(m_transform.position, 0)) * 
			glm::rotate(glm::radians(m_transform.rotation), glm::vec3(0, 0, 1));

		if (m_pParent)
			m_worldTransform = m_pParent->getWorldTransform() * localTransform; // This will recursively clean transforms up the tree.
		else
			m_worldTransform = localTransform;

		m_worldTransformWithScale = 
			m_worldTransform *
			glm::scale(glm::vec3(m_transform.scale.x, m_transform.scale.y, 1));

		m_invWorldTransform = glm::inverse(m_worldTransform);
		m_invWorldTransformWithScale = glm::inverse(m_worldTransformWithScale);

		m_transformDirty = false;
	}
	
	const glm::mat4& Entity::getWorldTransform()
	{
		updateDirtyTransforms();
		return m_worldTransform;
	}

	const glm::mat4& Entity::getInvWorldTransform()
	{
		updateDirtyTransforms();
		return m_invWorldTransform;
	}

	const glm::mat4& Entity::getWorldTransformWithScale()
	{
		updateDirtyTransforms();
		return m_worldTransformWithScale;
	}

	const glm::mat4& Entity::getInvWorldTransformWithScale()
	{
		updateDirtyTransforms();
		return m_invWorldTransformWithScale;
	}

	bool Entity::isInRadius(const glm::vec2 &pointInWorld, float radius, bool inclusive)
	{
		return inclusive
			? glm::distance(getWorldPosition(), pointInWorld) <= radius
			: glm::distance(getWorldPosition(), pointInWorld) < radius;
	}

	// Pretty slow, now partitionning, we basically check every entity/components :derp:
	EntityRef Entity::getMouseHover(const glm::vec2& mousePos, bool ignoreMouseFlags)
	{
		// We start with leaves first
		if (ignoreMouseFlags || mouseChildren)
		{
			if (sortChildren)
			{
				auto sorted = m_children;

				std::sort(sorted.begin(), sorted.end(), [](const EntityRef& a, const EntityRef& b)
				{
					return a->getWorldPosition().y < b->getWorldPosition().y;
				});

				for (auto rit = sorted.rbegin(); rit != sorted.rend(); ++rit)
				{
					const auto& pChild = *rit;
					auto pRet = pChild->getMouseHover(mousePos, ignoreMouseFlags);
					if (pRet) return pRet;
				}
			}
			else
			{
				for (auto rit = m_children.rbegin(); rit != m_children.rend(); ++rit)
				{
					const auto& pChild = *rit;
					auto pRet = pChild->getMouseHover(mousePos, ignoreMouseFlags);
					if (pRet) return pRet;
				}
			}
		}

		// Then self
		if (ignoreMouseFlags || !clickThrough)
			if (isMouseHover(mousePos))
				return shared_from_this();

		return nullptr;
	}

	bool Entity::isMouseHover(const glm::vec2& mousePos) const
	{
		for (const auto& pComponent : m_components)
		{
			if (pComponent->isEnabled())
				if (pComponent->isMouseHover(mousePos))
					return true;
		}
		return false;
	}

	void Entity::onMouseEnter()
	{
		for (const auto& pComponent : m_components)
			if (pComponent->isEnabled())
				pComponent->onMouseEnter();
	}

	void Entity::onMouseLeave()
	{
		for (const auto& pComponent : m_components)
			if (pComponent->isEnabled())
				pComponent->onMouseLeave();
	}

	void Entity::drawOutline(const glm::vec4& color, float zoomScale)
	{
		for (const auto& pComponent : m_components)
			pComponent->drawOutline(color, zoomScale);
	}

	bool Entity::edit()
	{
		bool changed = false;
		auto transformBefore = m_transform;

		GUI::idProperty("ID", id);
		changed |= GUI::stringProperty("Name", &name);
		changed |= GUI::boolProperty("Sort Children", &sortChildren, "Immediate children will be sorted Top to Bottom on the Y axis.");
		changed |= GUI::boolProperty("Mouse Children", &mouseChildren, "Allow mouse to interact with children.");
		changed |= GUI::boolProperty("Click Through", &clickThrough, "Mouse interaction will ignore this entity, but not its children.");
		changed |= GUI::boolProperty("UI Root", &uiRoot, "This entity will ignore world camera position, will act as root for UI.");

		GUI::beginGroup("Transform");
		{
			changed |= GUI::vec2Property("Position", &m_transform.position);
			changed |= GUI::angleProperty("Rotation", &m_transform.rotation);
			
			if (lockScale)
			{
				changed |= GUI::floatProperty("Scale", &m_transform.scale.x);
				m_transform.scale.y = m_transform.scale.x;
			}
			else
				changed |= GUI::vec2Property("Scale", &m_transform.scale);
			changed |= GUI::boolProperty("Lock Scale", &lockScale);
		}
		GUI::endGroup();

		ComponentRef pComponentToRemove = nullptr;
		int moveUpIndex = -1;
		int moveDownIndex = -1;
		int i = 0;
		for (auto it = m_components.begin(); it != m_components.end(); ++it, ++i)
		{
			const auto& pComponent = *it;

			GUI::SectionState sectionState = GUI::beginSection(pComponent->getType());
			switch (sectionState)
			{
				case GUI::SectionState::Open:
					changed |= pComponent->edit();
					GUI::endSection();
					break;
				case GUI::SectionState::Delete:
					pComponentToRemove = pComponent;
					break;
				case GUI::SectionState::MoveUp:
					moveUpIndex = i;
					break;
				case GUI::SectionState::MoveDown:
					moveDownIndex = i;
					break;
			}
		}

		if (pComponentToRemove)
		{
			removeComponent(pComponentToRemove);
			changed = true;
		}
		else if (moveUpIndex != -1)
		{
			if (moveUpIndex > 0)
			{
				std::swap(m_components[moveUpIndex], m_components[moveUpIndex - 1]);
				changed = true;
			}
		}
		else if (moveDownIndex != -1)
		{
			if (moveDownIndex + 1 < (int)m_components.size())
			{
				std::swap(m_components[moveDownIndex], m_components[moveDownIndex + 1]);
				changed = true;
			}
		}
		
		if (changed || transformBefore != m_transform) setDirtyTransform(); // Just dirty our transform whatever something changed
		return changed;
	}

	void Entity::draw()
	{
		for (const auto& pComponent : m_components)
			pComponent->draw();

		if (sortChildren)
		{
			auto sorted = m_children;

			std::sort(sorted.begin(), sorted.end(), [](const EntityRef& a, const EntityRef& b)
			{
				return a->getWorldPosition().y < b->getWorldPosition().y;
			});

			for (const auto& pChild : sorted)
				pChild->draw();
		}
		else
		{
			for (const auto& pChild : m_children)
				pChild->draw();
		}
	}
}
