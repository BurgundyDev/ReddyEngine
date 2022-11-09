#pragma once

#include <memory>

#include <json/value.h>

namespace Engine 
{
	class Entity;

	class Component
	{
	public:
		Component();
		~Component();

		Entity* entity;

		void update(float deltaTime);
		void fixedUpdate(float deltaTime);

		virtual void onCreate() = 0;
		virtual void onDestroy() = 0;
		virtual void onEnable() = 0;
		virtual void onDisable() = 0;
		
		void enable();
		void disable();

		Json::Value serialize();
		void deserialize(Json::Value json);
	private:
		bool isEnabled = false;
	};
}


