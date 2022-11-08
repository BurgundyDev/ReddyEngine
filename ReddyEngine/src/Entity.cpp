#include "Engine/Entity.h"
#include "Engine/Component.h"
#include "Engine/Utils.h"
#include "Engine/Log.h"
#include "Engine/EntityManager.h"
#include "Engine/ReddyEngine.h"
#include "ComponentManager.h"

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
				getEntityManager()->getComponentManager()->removeComponent(pComponent);
				m_components.erase(it);
				return true;
			}
		}
		return false;
	}

	void Entity::componentAdded(const ComponentRef& pComponent)
	{
		getEntityManager()->getComponentManager()->addComponent(pComponent);
	}

	Json::Value Entity::serialize()
	{
		Json::Value json;

		json["id"] = id;
		json["name"] = name;

		Json::Value componentsJson(Json::arrayValue);
		for (const auto& pComponent : m_components)
			componentsJson.append(pComponent->serialize());
		json["components"] = componentsJson;

		return json;
	}

	void Entity::deserialize(const Json::Value json)
	{
		id = Utils::deserializeUInt64(json["id"]);
		name = Utils::deserializeString(json["name"]);

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
			m_components.push_back(pComponent);
			getEntityManager()->getComponentManager()->addComponent(pComponent);
		}
	}

	void Entity::setTransform(const Transform transform)
	{
		m_transform = transform;
		m_transformDirty = true;
	}

	void Entity::edit()
	{
	}
}
