#include "Engine/PFXComponent.h"
#include "Engine/Utils.h"
#include "Engine/PFX.h"
#include "Engine/ResourceManager.h"
#include "Engine/ReddyEngine.h"
#include "Engine/Constants.h"
#include "Engine/GUI.h"
#include "Engine/Entity.h"
#include "Engine/SpriteBatch.h"
#include "Engine/Scene.h"

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
        json["destroyWhenDone"] = Utils::serializeJsonValue(destroyWhenDone);

        return json;
    }

    void PFXComponent::deserialize(const Json::Value& json)
    {
        Component::deserialize(json);

        pPFX = getResourceManager()->getPFX(Utils::deserializeString(json["pfx"]));
        triggerOnStart = Utils::deserializeBool(json["triggerOnStart"], true);
        destroyWhenDone = Utils::deserializeBool(json["destroyWhenDone"], false);
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

            if (destroyWhenDone && !getScene()->isEditorScene())
                if (!pPFXInstance->isAlive())
                    getScene()->destroyEntity(m_pEntity->shared_from_this());
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
    
    std::string PFXComponent::getFriendlyName() const
    {
        if (pPFX)
            return Utils::getFilenameWithoutExtension(pPFX->getFilename());
        return "";
    }

    bool PFXComponent::edit()
    {
        bool changed = false;

        if (ImGui::Button("Play")) play();
        ImGui::SameLine();
        if (ImGui::Button("Stop")) stop();

        changed |= GUI::PFXProperty("PFX", &pPFX);
        changed |= GUI::boolProperty("Trigger on Start", &triggerOnStart);
        changed |= GUI::boolProperty("Destroy when Done", &destroyWhenDone);

        return changed;
    }

    void PFXComponent::draw()
    {
        if (pPFXInstance)
            pPFXInstance->draw(m_pEntity->getWorldPosition(), m_pEntity->getRotation(), m_pEntity->getScale().x);
    }
}
