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
		DECLARE_COMPONENT("Sprite");

    public:
		SpriteComponent();
		~SpriteComponent() {}

		Json::Value serialize() override;
		void deserialize(const Json::Value& json) override;
		bool edit() override;
		void draw() override;

		bool isMouseHover(const glm::vec2& mousePos) const override;
		void drawOutline(const glm::vec4& color, float zoomScale) override; // For editor
		TextureRef getEditorIcon() const override;
		std::string getFriendlyName() const override;

		TextureRef pTexture;
		glm::vec4 color = {1, 1, 1, 1};
		glm::vec2 origin = {0.5f, 0.5f};
		glm::vec4 uvs = {0, 0, 1, 1};
		float additive = 0.0f;
    };
}
