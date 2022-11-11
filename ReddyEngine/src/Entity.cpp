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

	bool Entity::addChild(EntityRef pChild)
	{
		if (pChild->m_pParent) pChild->m_pParent->removeChild(pChild); // This could potentially make the pChild shared_ptr const reference invalid, that's why we pass by value
		for (const auto& pMyChild : m_children) if (pMyChild == pChild) return false;
		m_children.push_back(pChild);
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

	Json::Value Entity::serialize()
	{
		Json::Value json;

		json["id"] = id;
		json["name"] = name;
		json["sortChildren"] = sortChildren;
		json["mouseChildren"] = mouseChildren;
		json["uiRoot"] = uiRoot;
		json["transform"]["position"] = Utils::serializeJsonValue(m_transform.position);
		json["transform"]["rotation"] = Utils::serializeJsonValue(m_transform.rotation);
		json["transform"]["scale"] = Utils::serializeJsonValue(m_transform.scale);

		// Components
		Json::Value componentsJson(Json::arrayValue);
		for (const auto& pComponent : m_components)
			componentsJson.append(pComponent->serialize());
		json["components"] = componentsJson;

		// Children
		Json::Value childrenJson(Json::arrayValue);
		for (const auto& pChild : m_children)
			childrenJson.append(pChild->serialize());
		json["children"] = childrenJson;

		return json;
	}

	void Entity::deserialize(const Json::Value json)
	{
		for (const auto& pComponent : m_components)
		{
			getScene()->getComponentManager()->removeComponent(pComponent);
		}
		m_components.clear();

		id = Utils::deserializeUInt64(json["id"]);
		name = Utils::deserializeString(json["name"]);
		sortChildren = Utils::deserializeBool(json["sortChildren"], false);
		mouseChildren = Utils::deserializeBool(json["mouseChildren"], true);
		uiRoot = Utils::deserializeBool(json["uiRoot"], false);

		// Transform
		m_transform.position = Utils::deserializeJsonValue<glm::vec2>(json["transform"]["position"]);
		m_transform.rotation = Utils::deserializeFloat(json["transform"]["rotation"], 0.0f);
		const float DEFAULT_SCALE[2] = { 1.0f, 1.0f };
		Utils::deserializeFloat2(&m_transform.scale.x, json["transform"]["scale"], DEFAULT_SCALE);
		m_transformDirty = true;

		// Components
		const auto& componentsJson = json["components"];
		for (const auto& componentJson : componentsJson)
		{
			std::string type = Utils::deserializeString(componentJson["type"]);
			auto pComponent = Component::create(type);
			if (!pComponent)
			{
				CORE_ERROR("Unkonwn componentType: %s", type.c_str());
				continue;
			}

			pComponent->deserialize(componentJson);
			pComponent->m_pEntity = this;
			m_components.push_back(pComponent);
			componentAdded(pComponent);
		}

		// Children
		const auto& childrenJson = json["children"];
		for (const auto& childJson : childrenJson)
		{
			getScene()->createEntityFromJson(shared_from_this(), childJson);
		}
	}

	glm::vec2 Entity::getWorldPosition()
	{
		const auto& worldTransform = getWorldTransform();
		return worldTransform[3];
	}
	
	void Entity::setTransform(const Transform transform)
	{
		m_transform = transform;
		m_transformDirty = true;
	}

	void Entity::setPosition(const glm::vec2& position)
	{
		m_transform.position = position;
		m_transformDirty = true;
	}

	void Entity::setRotation(float degrees)
	{
		m_transform.rotation = degrees;
		m_transformDirty = true;
	}

	void Entity::setScale(const glm::vec2& scale)
	{
		m_transform.scale = scale;
		m_transformDirty = true;
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
			glm::scale(glm::vec3(m_transform.scale.x, m_transform.scale.y, 0));

		m_invWorldTransform = glm::inverse(m_worldTransform);

		m_transformDirty = false;
	}
	
	const glm::mat4& Entity::getWorldTransform()
	{
		updateDirtyTransforms();
		return m_worldTransform;
	}

	const glm::mat4& Entity::getWorldTransformWithScale()
	{
		updateDirtyTransforms();
		return m_worldTransformWithScale;
	}

	bool Entity::edit()
	{
		bool changed = false;

		GUI::idProperty("ID", id);
		changed |= GUI::stringProperty("Name", &name);
		changed |= GUI::boolProperty("Sort Children", &sortChildren, "Immediate children will be sorted Top to Bottom on the Y axis.");
		changed |= GUI::boolProperty("Mouse Children", &mouseChildren, "Allow mouse to interact with children.");
		changed |= GUI::boolProperty("UI Root", &uiRoot, "This entity will ignore world camera position, will act as root for UI.");

		GUI::beginGroup("Transform");
		{
			auto transformBefore = m_transform;

			changed |= GUI::vec2Property("Position", &m_transform.position);
			changed |= GUI::angleProperty("Rotation", &m_transform.rotation);
			
			static bool lockScale = true;
			if (lockScale)
			{
				changed |= GUI::floatProperty("Scale", &m_transform.scale.x);
				m_transform.scale.y = m_transform.scale.x;
			}
			else
				changed |= GUI::vec2Property("Scale", &m_transform.scale);
			changed |= GUI::boolProperty("Lock Scale", &lockScale);

			if (transformBefore != m_transform) setDirtyTransform(); // Just dirty our transform whatever something changed
		}
		GUI::endGroup();

		for (auto it = m_components.begin(); it != m_components.end(); ++it)
		{
			const auto& pComponent = *it;

			if (GUI::beginSection(pComponent->getType()) == GUI::SectionState::Open)
			{
				changed |= pComponent->edit();
				GUI::endSection();
			}
		}
		
		return changed;
	}

	void Entity::draw()
	{
		for (const auto& pComponent : m_components)
			pComponent->draw();

		for (const auto& pChild : m_children)
			pChild->draw();
	}
}
