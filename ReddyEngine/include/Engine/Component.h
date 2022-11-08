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

		virtual void onCreate() {};
		virtual void onDestroy() {};
		virtual void onEnable() {};
		virtual void onDisable() {};

		virtual Json::Value serialize();
		virtual void deserialize(const Json::Value& json);

		virtual std::string getType() const = 0;

		EntityRef getEntity();
		bool isEnabled() const { return m_isEnabled; }
		
		void enable();
		void disable();

	public:
		virtual bool edit() { return false; } // For editor, returns true if the Inspector modified a value

	private:
		friend class Entity;

		bool m_isEnabled = true;
		Entity* m_pEntity = nullptr;
	};
}
