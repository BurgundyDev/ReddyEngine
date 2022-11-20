#include "Engine/FrameAnimComponent.h"
#include "Engine/Utils.h"
#include "Engine/Texture.h"
#include "Engine/ResourceManager.h"
#include "Engine/ReddyEngine.h"
#include "Engine/Constants.h"
#include "Engine/GUI.h"
#include "Engine/Entity.h"
#include "Engine/SpriteBatch.h"
#include "Engine/SpriteComponent.h"
#include "Engine/ScriptComponent.h"
#include "Engine/Scene.h"
#include "Engine/Log.h"


namespace Engine
{
    FrameAnimComponent::FrameAnimComponent()
        : FrameAnimComponent(nullptr)
    {
    }

    FrameAnimComponent::FrameAnimComponent(const FrameAnimRef& frameAnim)
        : m_currentTime(0.0f),
          m_lastFrameTime(0.0f),
          m_frameIndex(-1),
          m_frameAnim(frameAnim)
    {
        setToFirstAnimation();
    }

    Json::Value FrameAnimComponent::serialize()
    {
        auto json = Component::serialize();

        if (m_frameAnim != nullptr) {
            Json::Value frameAnimJson;

            Json::ArrayIndex animationIndex = 0;

            for (const auto& anim : m_frameAnim->animationSet) {
                Json::Value framesJson;

                framesJson["name"] = anim.first;

                for (Json::ArrayIndex index = 0; index < static_cast<Json::ArrayIndex>(anim.second.frames.size()); index++) {
                    const auto& frame = anim.second.frames[index];

                    Json::Value frameJson;

                    if (frame.texture) {
                        frameJson["texture"] = frame.texture->getFilename();
                    }

                    frameJson["length"] = frame.length;

                    framesJson["frames"].insert(index, std::move(frameJson));
                }

                frameAnimJson["animations"].insert(animationIndex++, std::move(framesJson));
            }

            std::string filename = Utils::getFilenameWithoutExtension(m_frameAnim->getFilename());

            if (filename.empty()) {
                filename = Component::getFriendlyName();

                if (!filename.empty()) {
                    filename += "_";
                }

                std::time_t now = std::time(nullptr);

                filename += std::to_string(std::int64_t(now) / 1000);
                filename += "_";
                filename += std::to_string(std::uint8_t(std::rand() % UINT8_MAX));
            }

            const std::string filePath = std::string("frame_anims/") + filename + ".json";

            if (Utils::saveJson(frameAnimJson, "assets/" + filePath)) {
                json["frameAnim"] = filePath;
            } else {
                CORE_ERROR_POPUP("Failed to save frame animation at path %s!", filename.c_str());
            }
        }

        return json;
    }

    void FrameAnimComponent::deserialize(const Json::Value& json)
    {
        Component::deserialize(json);

        if (!json["frameAnim"].empty()) {
            const Json::Value& frameAnimPath = json["frameAnim"];

            m_frameAnim = getResourceManager()->getFrameAnim(frameAnimPath.asString());
        }

        if (!m_frameAnim) {
            m_frameAnim = FrameAnim::createFromJson(json);
        }

        if (m_frameAnim) {
            const Json::Value& currentAnimName = json["currentAnimName"];

            if (!currentAnimName.empty()) {
                setCurrentAnimation(currentAnimName.asString());
            } else {
                setToFirstAnimation();
            }
        }
    }

    bool FrameAnimComponent::edit()
    {
        bool changed = false;

        bool hasSpriteComponent = false;

        if (m_pEntity != nullptr) {
            hasSpriteComponent = m_pEntity->hasComponent<SpriteComponent>();
        }

        changed |= GUI::frameAnimProperty("Frame animation", &m_frameAnim, "Select the .json file that holds the animations and their frames.", !hasSpriteComponent);

        if (changed) {
            setToFirstAnimation();
        }

        return changed;
    }

    bool FrameAnimComponent::isMouseHover(const glm::vec2& mousePos) const
    {
        if (!m_frameAnim) return Component::isMouseHover(mousePos);
        
        const auto& invTransform = m_pEntity->getInvWorldTransformWithScale();

        glm::ivec2 textureSize = m_currentTexture ? m_currentTexture->getSize() : glm::ivec2{ 1, 1 };
        glm::vec2 sizef = glm::vec2((float)textureSize.x, (float)textureSize.y) * m_pEntity->getTransform().scale * SPRITE_BASE_SCALE;
        glm::vec2 invOrigin(1.f - origin.x, 1.f - origin.y);

        glm::vec2 localMouse = invTransform * glm::vec4(mousePos, 0, 1);

        return 
            localMouse.x >= -sizef.x * origin.x &&
            localMouse.x <= sizef.x * invOrigin.x &&
            localMouse.y >= -sizef.y * origin.y &&
            localMouse.y <= sizef.y * invOrigin.y;

        return false;
    }

	void FrameAnimComponent::drawOutline(const glm::vec4& color, float zoomScale)
    {
		const auto& transform = m_pEntity->getWorldTransformWithScale();
		auto sb = getSpriteBatch().get();

        glm::ivec2 textureSize = m_currentTexture ? m_currentTexture->getSize() : glm::ivec2{ 1, 1 };
        glm::vec2 sizef = glm::vec2((float)textureSize.x, (float)textureSize.y) * m_pEntity->getTransform().scale * SPRITE_BASE_SCALE;
        glm::vec2 invOrigin(1.f - origin.x, 1.f - origin.y);

        glm::vec2 points[4] = {
            transform * glm::vec4(-sizef.x * origin.x, -sizef.y * origin.y, 0, 1),
            transform * glm::vec4(-sizef.x * origin.x, sizef.y * invOrigin.y, 0, 1),
            transform * glm::vec4(sizef.x * invOrigin.x, sizef.y * invOrigin.y, 0, 1),
            transform * glm::vec4(sizef.x * invOrigin.x, -sizef.y * origin.y, 0, 1)
        };

		sb->drawLine(points[0], points[1], 2.0f * zoomScale, color);
        sb->drawLine(points[1], points[2], 2.0f * zoomScale, color);
        sb->drawLine(points[2], points[3], 2.0f * zoomScale, color);
        sb->drawLine(points[3], points[0], 2.0f * zoomScale, color);
    }

    TextureRef FrameAnimComponent::getEditorIcon() const
    {
        return Component::getEditorIcon();
    }

    
    void FrameAnimComponent::setCurrentAnimation(const std::string& animation)
    {
        m_currentTexture.reset();
        m_currentAnimation.clear();
        m_frameIndex = 0;
        m_lastFrameTime = 0.0f;
        m_currentTime = 0.0f;

        if (m_frameAnim == nullptr) {
            return;
        }

        const auto it = m_frameAnim->animationSet.find(animation);

        if (it == m_frameAnim->animationSet.end()) {
            m_currentAnimation.clear();

            return;
        }

        m_currentAnimation = animation;
    }

    void FrameAnimComponent::setToFirstAnimation()
    {
        m_currentTexture.reset();
        m_currentAnimation.clear();
        m_frameIndex = 0;
        m_lastFrameTime = 0.0f;
        m_currentTime = 0.0f;

        if (m_frameAnim == nullptr) {
            return;
        }

        for (const auto &it : m_frameAnim->animationSet) {
            setCurrentAnimation(it.first);

            return;
        }
    }

    void FrameAnimComponent::update(float dt)
    {
        if (getScene()->isEditorScene() || !updateFrameAnimation(dt)) {
            return;
        }
    }

    void FrameAnimComponent::draw()
    {
        // allow updating the animation in editor
        if (getScene()->isEditorScene()) {
            updateFrameAnimation(0.016f);
        }

        if (auto spriteComponent = m_pEntity->getComponent<SpriteComponent>()) {
            spriteComponent->pTexture = m_currentTexture;
        }

        // if in editor, and no current icon is visible, draw the icon
        if (getScene()->isEditorScene() && m_currentTexture == nullptr) {
            Component::draw();
        }
    }
    
    bool FrameAnimComponent::updateFrameAnimation(float dt)
    {
        if (m_currentAnimation.empty()) {
            return false;
        }

        if (m_frameIndex < 0) {
            m_frameIndex = 0;
            m_currentTime = 0.0f;
        }

        if (m_frameAnim == nullptr) {
            return false;
        }

        const auto& anim = m_frameAnim->animationSet[m_currentAnimation];

        if (anim.frames.empty()) {
            return false;
        }

        m_currentTexture = anim.frames[m_frameIndex].texture;

        m_currentTime += dt;

        if (m_currentTime - m_lastFrameTime >= anim.frames[m_frameIndex].length) {
            m_lastFrameTime = m_currentTime;
            ++m_frameIndex;
        }

        if (m_frameIndex >= anim.frames.size()) {
            m_frameIndex = 0;
            m_lastFrameTime = 0.0f;
            m_currentTime = 0.0f;
        }

        return true;
    }
}
