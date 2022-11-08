#include "Engine/Entity.h"
#include "Engine/Component.h"
#include "Engine/Utils.h"

#include <functional>

namespace Engine
{
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

	const Json::Value Entity::serialize()
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
