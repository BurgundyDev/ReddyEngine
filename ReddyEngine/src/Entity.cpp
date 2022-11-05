#include "Engine/Entity.h"

#include <functional>

namespace Engine
{
	std::shared_ptr<int> Entity::s_id = std::make_shared<int>(0);

	Entity::Entity(EntityShared& parent) : id(std::make_shared<int>(++(*s_id))),
		parent(parent)
	{

	}

	Entity::Entity() : id(std::make_shared<int>(++(*s_id)))
	{
		parent = nullptr;
	}

	Entity::~Entity()
	{

	}

	template<typename T>
	bool Entity::hasComponent() const
	{
		for (auto it = components.begin(); it != components.end(); it++)
		{
			if (it->is_class<T>()) return true;
		}
	}

	template<typename T, typename...TArgs>
	T& Entity::addComponent(TArgs&&... mArgs)
	{
		T* c(new T(std::forward<TArgs>(mArgs)...));
		c->entity = this;
		std::unique_ptr<Component> compPtr(c);
		components->push_back(compPtr);

		*compPtr->onCreate();
	}

	template<typename T>
	T& Entity::getComponent() const
	{
		T* c(new T(std::forward<TArgs>(mArgs)...));
		c->entity = this;
		std::unique_ptr<Component> uPtr(c);
		components->push_back(c);
	}

	void Entity::update(float deltatime)
	{

	}

	void Entity::fixedUpdate(float deltatime)
	{

	}

	void Entity::onCreate()
	{

	}

	void Entity::onDestroy()
	{

	}

	Json::Value Entity::serialize()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void Entity::deserialize(Json::Value json)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}
}