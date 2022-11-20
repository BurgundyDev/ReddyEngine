#pragma once

#include <Engine/Component.h>
#include <Engine/FrameAnim.h>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <vector>
#include <string>
#include <unordered_map>

namespace Engine
{
	class Texture;
	using TextureRef = std::shared_ptr<Texture>;

    class FrameAnimComponent final : public Component
    {
		DECLARE_COMPONENT("FrameAnim");

    public:
		FrameAnimComponent();
		FrameAnimComponent(const FrameAnimRef &frameAnim);
		~FrameAnimComponent() {}

		Json::Value serialize() override;
		void deserialize(const Json::Value& json) override;
		bool edit() override;
		void draw() override;
		void update(float dt) override;

		bool isMouseHover(const glm::vec2& mousePos) const override;
		void drawOutline(const glm::vec4& color, float zoomScale) override; // For editor
		virtual TextureRef getEditorIcon() const override;

		const std::string& getCurrentAnimation() const { return m_currentAnimation; }
		void setCurrentAnimation(const std::string& animation);
		void setToFirstAnimation();

		const TextureRef &getCurrentTexture() const
			{ return m_currentTexture; }

		const FrameAnimRef &GetFrameAnim() const
			{ return m_frameAnim; }

		glm::vec2 origin = {0.5f, 0.5f};

	private:
		bool updateFrameAnimation(float dt);

		FrameAnimRef m_frameAnim;
		float m_currentTime;
		float m_lastFrameTime;
		int m_frameIndex;
		std::string m_currentAnimation;
		TextureRef m_currentTexture;
    };
}
