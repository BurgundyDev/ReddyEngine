#include "Engine/PFXComponent.h"
#include "Engine/Utils.h"
#include "Engine/PFX.h"
#include "Engine/ResourceManager.h"
#include "Engine/ReddyEngine.h"
#include "Engine/Constants.h"
#include "Engine/GUI.h"
#include "Engine/Entity.h"
#include "Engine/SpriteBatch.h"

#include <imgui.h>


namespace Engine
{
    PFXComponent::PFXComponent()
    {
        pPFX = getResourceManager()->getPFX("particles/defaultPFX.json");
    }

    Json::Value PFXComponent::serialize()
    {
        auto json = Component::serialize();

        json["pfx"] = pPFX ? pPFX->getFilename() : "";
        json["triggerOnStart"] = Utils::serializeJsonValue(triggerOnStart);

        return json;
    }

    void PFXComponent::deserialize(const Json::Value& json)
    {
        Component::deserialize(json);

        pPFX = getResourceManager()->getPFX(Utils::deserializeString(json["pfx"]));
        triggerOnStart = Utils::deserializeBool(json["triggerOnStart"], true);
    }
    
	void PFXComponent::onCreate()
    {
        if (triggerOnStart && pPFX)
        {
            pPFXInstance = std::make_shared<PFXInstance>(pPFX);
        }
    }

    void PFXComponent::update(float dt)
    {
        if (pPFXInstance)
        {
            pPFXInstance->update(dt);
        }
    }

	void PFXComponent::play()
    {
        if (pPFX) pPFXInstance = std::make_shared<PFXInstance>(pPFX);
    }

	void PFXComponent::stop()
    {
        pPFXInstance = nullptr;
    }

    bool PFXComponent::edit()
    {
        bool changed = false;

        if (ImGui::Button("Play")) play();
        ImGui::SameLine();
        if (ImGui::Button("Stop")) stop();

        changed |= GUI::PFXProperty("PFX", &pPFX);
        changed |= GUI::boolProperty("Trigger on Start", &triggerOnStart);

        return changed;
    }

    void PFXComponent::draw()
    {
        if (pPFXInstance)
            pPFXInstance->draw(m_pEntity->getWorldPosition(), m_pEntity->getRotation(), m_pEntity->getScale().x);
    }
}
