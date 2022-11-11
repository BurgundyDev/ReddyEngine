#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
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
		virtual void draw() {}

		virtual void onCreate() {};
		virtual void onDestroy() {};
		virtual void onEnable() {};
		virtual void onDisable() {};
		virtual void onMouseEnter() {}
		virtual void onMouseLeave() {}

		virtual Json::Value serialize();
		virtual void deserialize(const Json::Value& json);

		virtual std::string getType() const = 0;

		virtual bool isMouseHover(const glm::vec2& mousePos) const;
		virtual void drawOutline(const glm::vec4& color, float zoomScale); // For editor

		EntityRef getEntity();
		bool isEnabled() const { return m_isEnabled; }
		
		void enable();
		void disable();
		
	public:
		virtual bool edit() { return false; } // For editor, returns true if the Inspector modified a value

	protected:
		Entity* m_pEntity = nullptr;

	private:
		friend class Entity;

		bool m_isEnabled = true;
	};
}
