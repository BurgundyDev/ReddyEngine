#pragma once

#include <Engine/Component.h>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>


namespace Engine
{
	class Font;
	using FontRef = std::shared_ptr<Font>;


    class TextComponent final : public Component
    {
		DECLARE_COMPONENT("Text");

    public:
		TextComponent();
		~TextComponent() {}

		Json::Value serialize() override;
		void deserialize(const Json::Value& json) override;
		bool edit() override;
		void draw() override;

		bool isMouseHover(const glm::vec2& mousePos) const override;
		void drawOutline(const glm::vec4& color, float zoomScale) override; // For editor
		std::string getFriendlyName() const override;

		FontRef pFont;
		glm::vec4 color = {1, 1, 1, 1};
		glm::vec2 origin = {0.5f, 0.5f};
		float scale = 1.0f;
		std::string text;
    };
}
