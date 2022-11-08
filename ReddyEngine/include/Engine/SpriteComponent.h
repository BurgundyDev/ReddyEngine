#pragma once

#include <Engine/Component.h>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>


namespace Engine
{
	class Texture;
	using TextureRef = std::shared_ptr<Texture>;


    class SpriteComponent final : public Component
    {
    public:
		SpriteComponent() {}
		~SpriteComponent() {}

		void onEnable() override;
		void onDisable() override;

		Json::Value serialize() override;
		void deserialize(const Json::Value& json) override;

		std::string getType() const override { return "Sprite"; }

	private:
		TextureRef m_pTexture;
		glm::vec4 m_color = {1, 1, 1, 1};
		glm::vec2 m_origin = {0.5f, 0.5f};
    };
}
