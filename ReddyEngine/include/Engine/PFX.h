#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <json/json.h>

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


    enum class EmitterType
    {
        burst,
        continuous
    };


    template<typename T>
    class RangedPFXValue final
    {
    public:
        T startRange[2];
        T endRange[2];
        bool randomStart = false;
        bool randomEnd = false;

        Json::Value serialize() const
        {
            Json::Value json;

            json["randomStart"] = randomStart;
            json["randomEnd"] = randomEnd;

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

            startRange[0] = Utils::deserializeJsonValue<T>(json["startRange"][0]);
            startRange[1] = Utils::deserializeJsonValue<T>(json["startRange"][1]);

            endRange[0] = Utils::deserializeJsonValue<T>(json["randomEnd"][0]);
            endRange[1] = Utils::deserializeJsonValue<T>(json["randomEnd"][1]);
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
    };


    struct EmitterDef
    {
        EmitterType type = EmitterType::burst;
        float burstDuration = 0.0f;
        int burstAmount = 100; // For burst
        float spawnRate = 10.0f; // For continuous
        TextureRef pTexture;
        float spread = 360.0f;
        RangedPFXValue<glm::vec4> color = {{{0, 0, 0, 1}, {1, 1, 1, 1}}, {{0, 0, 0, 0}, {0, 0, 0, 0}}, true, false};
        bool endOnlyAffectAlpha = true;
        RangedPFXValue<float> additive = {{1, 1}, {0, 0}, false, false};
        RangedPFXValue<float> size = {{0, 5}, {5, 10}, true, true};
        RangedPFXValue<float> speed = {{50, 100}, {0, 0}, true, false};
        RangedPFXSingleValue<float> duration = {{0.5f, 1.5f}, false};
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
            glm::vec2 position;
            glm::vec4 colorStart;
            glm::vec4 colorEnd;
            float speedStart;
            float speedEnd;
            TextureRef pTexture;
        };

        struct Emitters
        {
            float progress;
            const EmitterDef* pEmitterRef;
        };

        void spawn(int count);

        bool m_isAlive = true;
        PFXWeak m_pPFX;
        std::vector<Emitters> m_emitters;
        Particle* m_particlePool = nullptr;
        int m_nextFromPool = 0;
        int m_poolSize = 0;
        Particle* m_pParticleHead = nullptr;
    };
}
