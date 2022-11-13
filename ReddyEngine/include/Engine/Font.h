#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <memory>
#include <string>
#include <unordered_map>

#include "Engine/Resource.h"


struct stbtt_fontinfo;
struct stbrp_context;
struct stbrp_node;


namespace Engine
{
    class Font;
    using FontRef = std::shared_ptr<Font>;

    class Texture;
    using TextureRef = std::shared_ptr<Texture>;


    class Font final : public Resource
    {
    public:
        static constexpr char* SUPPORTED_FORMATS[] = { "json" };

        static FontRef createFromFile(const std::string& filename);

        ~Font();

        void draw(const std::string& text,
                  const glm::vec2& position, 
                  const glm::vec4& color = {1, 1, 1, 1},
                  float rotation = 0.0f,
                  float scale = 1.0f,
                  const glm::vec2& align = {0.0f, 0.0f});

        //void draw(const std::string& text,
        //          const glm::mat4& transform, 
        //          const glm::vec4& color = {1, 1, 1, 1},
        //          float scale = 1.0f,
        //          const glm::vec2& align = {0.0f, 0.0f});

        glm::vec2 measure(const std::string& text);

    private:
        struct Char
        {
            glm::vec2 offset;
            float xAdvance;
            glm::vec4 uvs;
        };

        Font();

        bool addGlyph(int codepoint);
        void updateAtlas();

        TextureRef m_pAtlas; // For now lets just use 1 atlas. But we might want to eventually page this if we go chinese and bigger characters.
        uint8_t* m_pAtlasData = nullptr;
        stbtt_fontinfo* m_pInfo = nullptr;
        uint8_t* m_pFontData = nullptr; // Font file in memory

        // Metrics
        int m_height = 0;
        float m_scale = 0;
        int m_ascent = 0;
        int m_descent = 0;
        int m_lineGap = 0;

        std::unordered_map<int, Char*> m_chars;
        stbrp_context* m_pRectContext = nullptr;
        stbrp_node* m_pRectNodes = nullptr;
    };
}
