#include "Engine/Component.h"
#include "Engine/Log.h"
#include "Engine/Entity.h"
#include "Engine/Utils.h"
#include "Engine/Scene.h"
#include "Engine/ReddyEngine.h"
#include "Engine/SpriteBatch.h"
#include "Engine/SpriteComponent.h"
#include "Engine/TextComponent.h"
#include "Engine/ScriptComponent.h"
#include "Engine/PFXComponent.h"
#include "Engine/ResourceManager.h"
#include "Engine/Constants.h"


namespace Engine
{
	std::unordered_map<std::string, TextureRef> Component::cachedEditorIcons = { };

	ComponentRef Component::create(const std::string& className)
	{
		if (className == "Sprite") return std::make_shared<SpriteComponent>();
		if (className == "Text") return std::make_shared<TextComponent>();
		if (className == "Script") return std::make_shared<ScriptComponent>();
		if (className == "PFX") return std::make_shared<PFXComponent>();

		//CORE_ERROR("Unrecognized Component Type: {}", className); // This will be called often legitemaly by script, it's spammy
		return nullptr;
	}
	
	void Component::clearCachedEditorIcons()
	{
		cachedEditorIcons.clear();
	}

	Component::Component()
	{
	}

	Component::~Component()
	{
	}
	
	EntityRef Component::getEntity()
	{
		return m_pEntity ? m_pEntity->shared_from_this() : nullptr;
	}
	
	Entity* Component::getEntityRaw()
	{
		return m_pEntity;
	}

	void Component::enable()
	{
		if (!m_isEnabled)
		{
			m_isEnabled = true;
			onEnable();
		}
	}

	void Component::disable()
	{
		if (m_isEnabled)
		{
			m_isEnabled = false;
			onDisable();
		}
	}

	Json::Value Component::serialize()
	{
		Json::Value json;
		json["type"] = getType();
		json["enabled"] = m_isEnabled;
		return json;
	}

	void Component::deserialize(const Json::Value& json)
	{
		m_isEnabled = Utils::deserializeBool(json["enabled"], true);
	}

	TextureRef Component::getEditorIcon() const
	{
		std::string name = getType();
		
		std::transform(
			name.begin(),
			name.end(),
			name.begin(),
			[](char ch) { return std::tolower(ch); }
		);

		const auto it = cachedEditorIcons.find(name);

		if (it != cachedEditorIcons.end()) {
			return it->second;
		}

		// All our icons will be PNG
		//for (const char* extension : Texture::SUPPORTED_FORMATS) {
			if (TextureRef icon = getResourceManager()->getTexture("textures/editorIcon_" + name + ".png")) {
				cachedEditorIcons.insert({ name, icon });

				return icon;
			}
		//}

		// note that we add to cachedEditorIcons anyway,
		// don't want to keep trying to load every call even if a component has no editor icon
		cachedEditorIcons.insert({ name, TextureRef() });

		return nullptr;
	}

	bool Component::isMouseHover(const glm::vec2& mousePos) const
	{
		if (!getScene()->isEditorScene()) return false;

		auto worldPos = m_pEntity->getWorldPosition();
		return 
			mousePos.x >= worldPos.x - 0.2f &&
			mousePos.x <= worldPos.x + 0.2f &&
			mousePos.y >= worldPos.y - 0.2f &&
			mousePos.y <= worldPos.y + 0.2f;
	}

	void Component::drawOutline(const glm::vec4& color, float zoomScale)
	{
		auto worldPos = m_pEntity->getWorldPosition();
		auto sb = getSpriteBatch().get();

		sb->drawLine({worldPos.x - 0.2f, worldPos.y - 0.2f}, {worldPos.x - 0.2f, worldPos.y + 0.2f}, 2.0f * zoomScale, color);
		sb->drawLine({worldPos.x - 0.2f, worldPos.y + 0.2f}, {worldPos.x + 0.2f, worldPos.y + 0.2f}, 2.0f * zoomScale, color);
		sb->drawLine({worldPos.x + 0.2f, worldPos.y + 0.2f}, {worldPos.x + 0.2f, worldPos.y - 0.2f}, 2.0f * zoomScale, color);
		sb->drawLine({worldPos.x + 0.2f, worldPos.y - 0.2f}, {worldPos.x - 0.2f, worldPos.y - 0.2f}, 2.0f * zoomScale, color);
	}

	void Component::draw()
	{
		if (!getScene()->isEditorScene()) return;

		if (const TextureRef editorIcon = getEditorIcon()) {
			
			getSpriteBatch()->drawSprite(
				editorIcon,
				m_pEntity->getWorldTransformWithScale(),
                glm::vec4(1.0f),
                m_pEntity->getTransform().scale * SPRITE_BASE_SCALE
			);
		}
	}
}

