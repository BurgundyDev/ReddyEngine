#pragma once

#include "Engine/ISerializable.h"
#include "Engine/Component.h"

#include <glm/glm.hpp>

#include <string>
#include <memory>
#include <vector>

struct Transform 
{
	glm::fvec2 position;
	float rotation;
	glm::fvec2 scale;
};

namespace Engine
{
	using EntityShared = std::shared_ptr<Entity>;
	using ComponentShared = std::shared_ptr<Component>;
	using TransformShared = std::shared_ptr<Transform>;

	class Entity : public ISerializable
	{
	public:
		Entity(EntityShared& parent);
		Entity();
		~Entity();

		std::shared_ptr<int> id;

		EntityShared					parent;
		std::vector<EntityShared>		children;
		std::vector<ComponentShared>	components;

		TransformShared transform;
			
		void update(float deltatime);
		void fixedUpdate(float deltatime);
		void onCreate();
		void onDestroy();
		
		template<typename T>
		bool hasComponent() const;

		template<typename T, typename... TArgs>
		T& addComponent(TArgs&&... mArgs);

		template<typename T>
		T& getComponent() const;

		Json::Value serialize() override;
		void deserialize(Json::Value json) override;
		
		friend bool operator==(const Entity& lhs, const Entity& rhs)
		{
			return lhs.id == rhs.id;
		}
	private:
		static std::shared_ptr<int> s_id;
	};
}