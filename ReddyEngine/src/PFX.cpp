#include "Engine/PFX.h"
#include "Engine/ReddyEngine.h"
#include "Engine/ResourceManager.h"
#include "Engine/SpriteBatch.h"
#include "Engine/Texture.h"
#include "Engine/Utils.h"

#include <glm/glm.hpp>


namespace Engine
{
    PFXRef PFX::createFromFile(const std::string& filename)
    {
        Json::Value json;
        if (!Utils::loadJson(json, filename)) return nullptr;

        return createFromJson(json);
    }

    PFXRef PFX::createFromJson(const Json::Value& json)
    {
        auto pRet = std::shared_ptr<PFX>(new PFX());
        pRet->deserialize(json);
        return pRet;
    }

    PFXRef PFX::create()
    {
        auto pRet = std::shared_ptr<PFX>(new PFX());

        EmitterDef emitter;
        emitter.pTexture = getResourceManager()->getTexture("textures/particle.png");
        pRet->emitters.push_back(emitter);

        return pRet;
    }

    Json::Value PFX::serialize() const
    {
        Json::Value json;

        json["version"] = FILES_VERSION;

        Json::Value emittersJson(Json::arrayValue);
        for (const auto& emitter : emitters)
        {
            Json::Value emitterJson;

            switch (emitter.type)
            {
                case EmitterType::burst:
                    emitterJson["type"] = "burst";
                    break;
                case EmitterType::continuous:
                    emitterJson["type"] = "continuous";
                    break;
            }

            emitterJson["burstDuration"] = emitter.burstDuration;
            emitterJson["burstAmount"] = emitter.burstAmount;
            emitterJson["spawnRate"] = emitter.spawnRate;
            if (emitter.pTexture)
                emitterJson["texture"] = emitter.pTexture->getFilename();
            else
                emitterJson["texture"] = "";
            emitterJson["spread"] = emitter.spread;
            emitterJson["color"] = emitter.color.serialize();
            emitterJson["endOnlyAffectAlpha"] = emitter.endOnlyAffectAlpha;
            emitterJson["additive"] = emitter.additive.serialize();
            emitterJson["size"] = emitter.size.serialize();
            emitterJson["rotation"] = emitter.rotation.serialize();
            emitterJson["rotationSpeed"] = emitter.rotationSpeed.serialize();
            emitterJson["speed"] = emitter.speed.serialize();
            emitterJson["duration"] = emitter.duration.serialize();


            emittersJson.append(emitterJson);
        }

        json["emitters"] = emittersJson;

        return json;
    }

    void PFX::deserialize(const Json::Value& json)
    {
        int version = Utils::deserializeInt32(json["version"], FILES_VERSION);

        emitters.clear();

        const auto& emittersJson = json["emitters"];
        for (const auto& emitterJson : emittersJson)
        {
            EmitterDef emitter;

            auto type = Utils::deserializeString(emitterJson["type"], "burst");
            if (type == "burst") emitter.type = EmitterType::burst;
            else if (type == "continuous") emitter.type = EmitterType::continuous;
            else emitter.type = EmitterType::burst;

            emitter.burstDuration = Utils::deserializeFloat(emitterJson["burstDuration"], 0.0f);
            emitter.burstAmount = Utils::deserializeInt32(emitterJson["burstAmount"], 100);
            emitter.spawnRate = Utils::deserializeFloat(emitterJson["spawnRate"], 20.0f);
            emitter.pTexture = getResourceManager()->getTexture(Utils::deserializeString(emitterJson["texture"], "textures/particle.png"));
            emitter.spread = Utils::deserializeFloat(emitterJson["spread"], 360.0f);
            emitter.endOnlyAffectAlpha = Utils::deserializeBool(emitterJson["endOnlyAffectAlpha"], true);

            emitter.color.deserialize(emitterJson["color"]);
            emitter.additive.deserialize(emitterJson["additive"]);
            emitter.size.deserialize(emitterJson["size"]);
            emitter.rotation.deserialize(emitterJson["rotation"]);
            emitter.rotationSpeed.deserialize(emitterJson["rotationSpeed"]);
            emitter.speed.deserialize(emitterJson["speed"]);
            emitter.duration.deserialize(emitterJson["duration"]);

            emitters.push_back(emitter);
        }
    }
    
    PFXInstance::PFXInstance(const PFXRef& pPFX)
    {
        m_pPFX = pPFX;

        const auto& pfxEmitters = pPFX->emitters;

        m_emitters.resize(pfxEmitters.size());

        // Initialize emitters, and aproximate how many particles we'll need in total
        m_poolSize = 0;
        for (int i = 0, len = (int)m_emitters.size(); i < len; ++i)
        {
            auto pEmitter = &m_emitters[i];
            pEmitter->pEmitterRef = &pfxEmitters[i];
            pEmitter->progress = 0.0f;
            pEmitter->spawnAccum = 0.0f;
            pEmitter->spawnRate = pEmitter->pEmitterRef->spawnRate;
            if (pEmitter->pEmitterRef->type == EmitterType::burst && pEmitter->pEmitterRef->burstDuration > 0.0f)
            {
                pEmitter->spawnRate = (float)pEmitter->pEmitterRef->burstAmount / pEmitter->pEmitterRef->burstDuration;
            }

            switch (pEmitter->pEmitterRef->type)
            {
                case EmitterType::burst:
                    m_poolSize += pEmitter->pEmitterRef->burstAmount;
                    break;
                case EmitterType::continuous:
                    m_poolSize += (int)(pEmitter->pEmitterRef->spawnRate * std::max(pEmitter->pEmitterRef->duration.range[0], pEmitter->pEmitterRef->duration.range[1])) + 1;
                    break;
            }
        }

        m_poolSize += m_poolSize / 3; // Add a little extra for good luck
        m_particlePool = new Particle[m_poolSize];

        // If burst duration is 0, spawn them all right away
        for (const auto& emitter : m_emitters)
        {
            if (emitter.pEmitterRef->type == EmitterType::burst)
            {
                if (emitter.pEmitterRef->burstDuration == 0.0f)
                    PFXInstance::spawn(emitter.pEmitterRef->burstAmount, emitter);
            }
        }
    }

    PFXInstance::~PFXInstance()
    {
        delete[] m_particlePool;
    }

    PFXInstance::Particle* PFXInstance::createParticle()
    {
        int end = m_nextFromPool;
        Particle* pParticle = nullptr;

        do
        {
            auto pParticle = &m_particlePool[m_nextFromPool];
            m_nextFromPool = (m_nextFromPool + 1) % m_poolSize;

            if (!pParticle->inUse)
            {
                if (!m_pParticleHead)
                {
                    m_pParticleHead = pParticle;
                    m_pParticleHead->pNext = nullptr;
                }
                else
                {
                    pParticle->pNext = m_pParticleHead;
                    m_pParticleHead = pParticle;
                }
                pParticle->inUse = true;
                return pParticle;
            }

        } while (m_nextFromPool != end);

        return nullptr;
    }

    void PFXInstance::spawn(int count, const Emitter& emitter)
    {
        for (int i = 0; i < count; ++i)
        {
            auto pParticle = createParticle();
            if (!pParticle) return; // Ran out from the pool. (Shouldn't happen?)

            pParticle->progress = 0.0f;
            pParticle->position = {0, 0};
            pParticle->delay = 1.0f / emitter.pEmitterRef->duration.gen();
            pParticle->colorStart = emitter.pEmitterRef->color.genStart();
            pParticle->colorEnd = emitter.pEmitterRef->color.genEnd();

            if (emitter.pEmitterRef->endOnlyAffectAlpha)
            {
                pParticle->colorEnd.r = pParticle->colorStart.r;
                pParticle->colorEnd.g = pParticle->colorStart.g;
                pParticle->colorEnd.b = pParticle->colorStart.b;
            }

            pParticle->additiveStart = emitter.pEmitterRef->additive.genStart();
            pParticle->additiveEnd = emitter.pEmitterRef->additive.genEnd();
            pParticle->sizeStart = emitter.pEmitterRef->size.genStart();
            pParticle->sizeEnd = emitter.pEmitterRef->size.genEnd();
            pParticle->rotationSpeedStart = emitter.pEmitterRef->rotationSpeed.genStart();
            pParticle->rotationSpeedEnd = emitter.pEmitterRef->rotationSpeed.genEnd();
            pParticle->rotation = emitter.pEmitterRef->rotation.gen();
            pParticle->speedStart = emitter.pEmitterRef->speed.genStart();
            pParticle->speedEnd = emitter.pEmitterRef->speed.genEnd();
            pParticle->pTexture = emitter.pEmitterRef->pTexture;
            pParticle->texInvSize = emitter.pEmitterRef->pTexture ? 1.0f / (float)emitter.pEmitterRef->pTexture->getSize().x : 1.0f;
            {
                float angle = emitter.pEmitterRef->spread * ((float)(rand() % 10001) / 10000.0f);
                angle -= emitter.pEmitterRef->spread * 0.5f;
                auto radTheta = glm::radians(angle);
                auto sinTheta = std::sin(radTheta);
                auto cosTheta = std::cos(radTheta);
                pParticle->dir = {-sinTheta, -cosTheta};
            }
        }
    }

    void PFXInstance::update(float dt)
    {
        // Make sure PFX asset wasn't destroyed. If the case, we're not alive anymore
        if (!m_pPFX.lock())
        {
            m_isAlive = false;
            return;
        }

        // Update emitters
        for (auto& emitter : m_emitters)
        {
            if (emitter.pEmitterRef->type == EmitterType::burst)
            {
                emitter.progress += dt;
                if (emitter.progress >= emitter.pEmitterRef->burstDuration)
                {
                    continue;
                }
            }

            emitter.spawnAccum += dt / emitter.spawnRate;

            while (emitter.spawnAccum >= 1.0f)
            {
                spawn((int)emitter.spawnAccum, emitter);
                emitter.spawnAccum -= (float)(int)emitter.spawnAccum;
            }
        }

        // Update particles
        Particle* pPrevParticle = nullptr;
        Particle* pParticle = m_pParticleHead;
        while (pParticle)
        {
            pParticle->progress += pParticle->delay * dt;
            if (pParticle->progress >= 1.0f)
            {
                pParticle->inUse = false;
                if (pParticle == m_pParticleHead) m_pParticleHead = pParticle->pNext;
                if (pPrevParticle) pPrevParticle->pNext = pParticle->pNext;
            }

            auto speed = Utils::lerp(pParticle->speedStart, pParticle->speedEnd, pParticle->progress);
            pParticle->position += pParticle->dir * speed * dt;

            auto rotationSpeed = Utils::lerp(pParticle->rotationSpeedStart, pParticle->rotationSpeedEnd, pParticle->progress);
            pParticle->rotation += rotationSpeed * dt;

            pPrevParticle = pParticle;
            pParticle = pParticle->pNext;
        }
    }

    void PFXInstance::draw(const glm::vec2& position, float in_rotation, float scale)
    {
        if (!m_isAlive) return;

        auto sb = getSpriteBatch().get();

        Particle* pParticle = m_pParticleHead;
        while (pParticle)
        {
            auto t = pParticle->progress;
            auto color = Utils::lerp(pParticle->colorStart, pParticle->colorEnd, t);
            auto size = Utils::lerp(pParticle->sizeStart, pParticle->sizeEnd, t);
            auto additive = Utils::lerp(pParticle->additiveStart, pParticle->additiveEnd, t);

            glm::vec4 premultiplied(color.r * color.a, color.g * color.a, color.b * color.a, color.a * (1.0f - additive));

            color = Utils::lerp(premultiplied, color, additive);

            sb->draw(pParticle->pTexture, position + pParticle->position, color, pParticle->rotation + in_rotation, size * scale * 0.01f * pParticle->texInvSize);

            pParticle = pParticle->pNext;
        }
    }
}
