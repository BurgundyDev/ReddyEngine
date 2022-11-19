#pragma once

#include <Engine/Utils.h>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <json/json.h>

#include <stdlib.h>
#include <memory>
#include <string>
#include <vector>

#include "Resource.h"


namespace Engine
{
    class PFX;
    using PFXRef = std::shared_ptr<PFX>;
    using PFXWeak = std::weak_ptr<PFX>;

    class Texture;
    using TextureRef = std::shared_ptr<Texture>;


    enum class EmitterType : int
    {
        burst,
        continuous
    };


    template<class T>
    class RangedPFXValue final
    {
    public:
        T startRange[2];
        T endRange[2];
        bool randomStart = false;
        bool randomEnd = false;
        bool sameStartEnd = true;

        Json::Value serialize() const
        {
            Json::Value json;

            json["randomStart"] = randomStart;
            json["randomEnd"] = randomEnd;
            json["sameStartEnd"] = sameStartEnd;

            Json::Value startRangeJson(Json::arrayValue);
            startRangeJson.append(Utils::serializeJsonValue(startRange[0]));
            startRangeJson.append(Utils::serializeJsonValue(startRange[1]));
            json["startRange"] = startRangeJson;

            Json::Value endRangeJson(Json::arrayValue);
            endRangeJson.append(Utils::serializeJsonValue(endRange[0]));
            endRangeJson.append(Utils::serializeJsonValue(endRange[1]));
            json["endRange"] = endRangeJson;

            return json;
        }

        void deserialize(const Json::Value& json)
        {
            randomStart = Utils::deserializeBool(json["randomStart"], false);
            randomEnd = Utils::deserializeBool(json["randomEnd"], false);
            sameStartEnd = Utils::deserializeBool(json["sameStartEnd"], true);

            startRange[0] = Utils::deserializeJsonValue<T>(json["startRange"][0]);
            startRange[1] = Utils::deserializeJsonValue<T>(json["startRange"][1]);

            endRange[0] = Utils::deserializeJsonValue<T>(json["endRange"][0]);
            endRange[1] = Utils::deserializeJsonValue<T>(json["endRange"][1]);
        }

        T genStart() const
        {
            if (!randomStart) return startRange[0];
            float p = (float)(rand() % 10001) / 10000.0f;
            return startRange[0] + (startRange[1] - startRange[0]) * p;
        }

        T genEnd(const T& startValue) const
        {
            if (sameStartEnd) return startValue;
            if (!randomEnd) return endRange[0];
            float p = (float)(rand() % 10001) / 10000.0f;
            return endRange[0] + (endRange[1] - endRange[0]) * p;
        }
    };


    template<typename T>
    class RangedPFXSingleValue final
    {
    public:
        T range[2];
        bool random = false;

        Json::Value serialize() const
        {
            Json::Value json;

            json["random"] = random;

            Json::Value rangeJson(Json::arrayValue);
            rangeJson.append(Utils::serializeJsonValue(range[0]));
            rangeJson.append(Utils::serializeJsonValue(range[1]));
            json["range"] = rangeJson;

            return json;
        }

        void deserialize(const Json::Value& json)
        {
            random = Utils::deserializeBool(json["random"], false);

            range[0] = Utils::deserializeJsonValue<T>(json["range"][0]);
            range[1] = Utils::deserializeJsonValue<T>(json["range"][1]);
        }

        T gen() const
        {
            if (!random) return range[0];
            float p = (float)(rand() % 10001) / 10000.0f;
            return range[0] + (range[1] - range[0]) * p;
        }
    };


    struct EmitterDef
    {
        std::string name = "emitter";
        EmitterType type = EmitterType::burst;
        float burstDuration = 0.0f;
        int burstAmount = 100; // For burst
        float spawnRate = 10.0f; // For continuous
        TextureRef pTexture;
        float spread = 360.0f;
        RangedPFXValue<glm::vec4> color = {{{1, .3f, 0, 1}, {1, 1, 1, 1}}, {{0, 0, 0, 0}, {0, 0, 0, 0}}, true, false, false};
        bool endOnlyAffectAlpha = true;
        RangedPFXValue<float> additive = {{1, 1}, {0, 0}, false, false, false};
        RangedPFXSingleValue<float> rotation = {{0, 360}, true};
        RangedPFXValue<float> rotationSpeed = {{90, 180}, {0, 0}, true, false, false};
        RangedPFXValue<float> size = {{0, 5}, {50, 150}, true, true, false};
        RangedPFXValue<float> speed = {{300, 600}, {0, 0}, true, false, false};
        RangedPFXSingleValue<float> duration = {{0.5f, 1.5f}, true};
        RangedPFXValue<glm::vec2> gravity = {{{0, -100.0f}, {0, 0}}, {{0, 0}, {0, 0}}, false, false, true};
        RangedPFXSingleValue<float> spawnRadius = {{0.0f, 50.0f}, true};
    };


    class PFX final : public Resource
    {
    public:
        static PFXRef createFromFile(const std::string& filename);
        static PFXRef createFromJson(const Json::Value& json);
        static PFXRef create();

        std::vector<EmitterDef> emitters; // This is public so we can easily modify it

        Json::Value serialize() const;
        void deserialize(const Json::Value& json);

    private:
        PFX() {}
    };


    class PFXInstance final
    {
    public:
        PFXInstance(const PFXRef& pPFX);
        ~PFXInstance();

        bool isAlive() const { return m_isAlive; }
        void update(float dt);
        void draw(const glm::vec2& position, float rotation = 0.0f, float scale = 1.0f);

    private:
        struct Particle
        {
            Particle* pNext; // We use linked list because using std::vector there is too many insertion/deletion
            bool inUse = false;
            float progress;
            float delay;
            float rotation;
            TextureRef pTexture;
            float texInvSize;
            glm::vec2 position;
            glm::vec2 gravityStart;
            glm::vec2 gravityEnd;
            glm::vec2 dir;
            glm::vec4 colorStart;
            glm::vec4 colorEnd;
            float additiveStart;
            float additiveEnd;
            float sizeStart;
            float sizeEnd;
            float rotationSpeedStart;
            float rotationSpeedEnd;
            float speedStart;
            float speedEnd;
        };

        struct Emitter
        {
            const EmitterDef* pEmitterRef;
            float progress;
            float spawnAccum;
            float spawnRate;
        };

        void spawn(int count, const Emitter& emitter);
        Particle* createParticle();

        bool m_isAlive = true;
        PFXWeak m_pPFX;
        std::vector<Emitter> m_emitters;
        Particle* m_particlePool = nullptr;
        int m_nextFromPool = 0;
        int m_poolSize = 0;
        Particle* m_pParticleHead = nullptr;
    };
}
