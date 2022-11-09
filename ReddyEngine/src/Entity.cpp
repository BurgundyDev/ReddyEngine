#include "Engine/Entity.h"

#include <functional>

namespace Engine
{
	Entity::Entity(const EntityRef& parent) : parent(parent.get())
	{

	}

	Entity::Entity()
	{
		parent = nullptr;
	}

	Entity::~Entity()
	{
	}

	void Entity::update(float deltaTime)
	{
		auto childs = children;

		for (auto it = childs.begin(); it != childs.end(); it++)
		{
			(*it)->update(deltaTime);
		}
	}

	void Entity::fixedUpdate(float deltaTime)
	{
		auto childs = children;

		for (auto it = childs.begin(); it != childs.end(); it++)
		{
			(*it)->fixedUpdate(deltaTime);
		}
	}

	void Entity::onCreate()
	{
		
	}

	void Entity::onDestroy()
	{

	}

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
			getEntityManager()->getComponentManager()->removeComponent(pComponent);
		}
		m_components.clear();

		id = Utils::deserializeUInt64(json["id"]);
		name = Utils::deserializeString(json["name"]);

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
			getEntityManager()->createEntityFromJson(shared_from_this(), childJson);
		}
	}
}