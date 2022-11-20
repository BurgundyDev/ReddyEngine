#pragma once

#include <Engine/Component.h>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>


namespace Engine
{
	class Texture;
	using TextureRef = std::shared_ptr<Texture>;


	// This is a huge copy paste from sprite component, with padding added
    class Slice9Component final : public Component
    {
		DECLARE_COMPONENT("Slice9");

    public:
		Slice9Component();
		~Slice9Component() {}

		Json::Value serialize() override;
		void deserialize(const Json::Value& json) override;
		bool edit() override;
		void draw() override;

		bool isMouseHover(const glm::vec2& mousePos) const override;
		void drawOutline(const glm::vec4& color, float zoomScale) override; // For editor
		TextureRef getEditorIcon() const override;
		std::string getFriendlyName() const override;

		/*! \brief If the current texture is set by FrameAnimComponent, disable it from being changed */
		bool isFrameAnimTexture() const;

		TextureRef pTexture;
		glm::vec4 color = {1, 1, 1, 1};
		glm::vec2 origin = {0.5f, 0.5f};
		glm::vec4 uvs = {0, 0, 1, 1};
		float additive = 0.0f;
		glm::vec2 scale = {1, 1};
		glm::ivec4 padding = {0, 0, 0, 0};
		bool repeatCenter = false;
    };
}
