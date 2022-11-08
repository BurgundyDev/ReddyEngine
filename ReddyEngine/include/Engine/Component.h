#pragma once

#include <json/json.h>

#include <memory>


namespace Engine 
{
	class Entity;
	using EntityRef = std::shared_ptr<Entity>;


	class Component;
	using ComponentRef = std::shared_ptr<Component>;


	class Component
	{
	public:
		static ComponentRef create(const std::string& className);

		Component();
		virtual ~Component();

		virtual void update(float deltaTime) {}
		virtual void fixedUpdate(float deltaTime) {}

		virtual void onCreate() = 0;
		virtual void onDestroy() = 0;
		virtual void onEnable() = 0;
		virtual void onDisable() = 0;
		
		void enable();
		void disable();

		virtual Json::Value serialize() = 0;
		virtual void deserialize(Json::Value json) = 0;

		EntityRef getEntity();

	private:
		bool m_isEnabled = false;
		Entity* m_pEntity = nullptr;
	};
}


