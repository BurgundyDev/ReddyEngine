#pragma once

#include "Engine/ISerializable.h"

#include <memory>

namespace Engine 
{
	class Entity;

	class Component : ISerializable
	{
	public:
		Component();
		~Component();

		std::shared_ptr<Entity> entity;

		virtual void update(float deltaTime) = 0;
		virtual void fixedUpdate(float deltaTime) = 0;

		virtual void onCreate() = 0;
		virtual void onDestroy() = 0;
		virtual void onEnable() = 0;
		virtual void onDisable() = 0;
		
		void enable();
		void disable();

		Json::Value serialize() override;
		void deserialize(Json::Value json) override;
	private:
		bool isEnabled = true;
	};
}


