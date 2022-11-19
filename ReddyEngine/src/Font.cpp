#include "Engine/Font.h"
#include "Engine/Log.h"
#include "Engine/ReddyEngine.h"
#include "Engine/SpriteBatch.h"
#include "Engine/Texture.h"
#include "Engine/Utils.h"

#include <glm/glm.hpp>
#include <json/json.h>

#define STB_RECT_PACK_IMPLEMENTATION
#include <stb_rect_pack.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>


static const int ATLAS_SIZE = 1024;
static const int PACK_MAX_NODES = 1024; // Number of characters, that's not enough if we go chinese. Should be 12000 at least..
static const int PADDING = 2;


namespace Engine
{
    FontRef Font::createFromFile(const std::string& filename)
    {
        Json::Value json;
        Utils::loadJson(json, filename);

        auto pRet = std::shared_ptr<Font>(new Font());
        pRet->m_height = Utils::deserializeInt32(json["height"], 24);
        pRet->m_shadowAlpha = Utils::deserializeFloat(json["shadowOpacity"], 0.0f);
        pRet->m_shadowDistance = Utils::deserializeInt32(json["shadowDistance"], 2);
        
        // load font file
        long size;
    
        auto fontFilename = "assets/" + json["file"].asString();
        FILE* fontFile = fopen(fontFilename.c_str(), "rb");
        if (!fontFile)
        {
            CORE_ERROR("Failed to load Font: {}", fontFilename.c_str());
            return nullptr;
        }

        fseek(fontFile, 0, SEEK_END);
        size = ftell(fontFile); 
        fseek(fontFile, 0, SEEK_SET);
        pRet->m_pFontData = new uint8_t[size];
        fread(pRet->m_pFontData, 1, size, fontFile);
        fclose(fontFile);

        // Create dynamic texture where we will put in the glyphs
        pRet->m_pAtlas = Texture::createDynamic({ATLAS_SIZE, ATLAS_SIZE});

        // Init packing
        pRet->m_pRectContext = new stbrp_context();
        pRet->m_pRectNodes = new stbrp_node[PACK_MAX_NODES];
        stbrp_init_target(pRet->m_pRectContext, ATLAS_SIZE, ATLAS_SIZE, pRet->m_pRectNodes, PACK_MAX_NODES);

        // Prepare font
        if (!stbtt_InitFont(pRet->m_pInfo, pRet->m_pFontData, 0))
        {
            CORE_ERROR("Failed to prepare Font: {}", fontFilename.c_str());
            return nullptr;
        }

        // Get font metrics
        pRet->m_scale = stbtt_ScaleForPixelHeight(pRet->m_pInfo, (float)pRet->m_height);
        stbtt_GetFontVMetrics(pRet->m_pInfo, &pRet->m_ascent, &pRet->m_descent, &pRet->m_lineGap);
        pRet->m_ascent = (int)std::roundf(pRet->m_ascent * pRet->m_scale);
        pRet->m_descent = (int)std::roundf(pRet->m_descent * pRet->m_scale);

        // Generate glyphs for most common characters
        for (int i = ' '; i <= '~'; ++i)
        {
            pRet->addGlyph(i);
        }
        pRet->updateAtlas();

        return pRet;
     }
    
    Font::Font()
    {
        m_pAtlasData = new uint8_t[ATLAS_SIZE * ATLAS_SIZE * 4];
        memset(m_pAtlasData, 0, ATLAS_SIZE * ATLAS_SIZE * 4);
        m_pInfo = new stbtt_fontinfo();
    }

    Font::~Font()
    {
        for (auto kv : m_chars) delete kv.second;
        delete[] m_pAtlasData;
        delete m_pInfo;
        delete m_pRectContext;
        delete[] m_pFontData;
    }

    bool Font::addGlyph(int codepoint)
    {
        int shadowPadding = m_shadowAlpha > 0.0f ? m_shadowDistance : 0;
        int shadowAlpha = (int)(m_shadowAlpha * 255.0f);

        // how wide is this character
        int ax;
	    int lsb;
        stbtt_GetCodepointHMetrics(m_pInfo, codepoint, &ax, &lsb);

        // get bounding box for character
        int c_x1, c_y1, c_x2, c_y2;
        stbtt_GetCodepointBitmapBox(m_pInfo, codepoint, m_scale, m_scale, &c_x1, &c_y1, &c_x2, &c_y2);
        int w = c_x2 - c_x1;
        int h = c_y2 - c_y1;

        // Find space in our atlas
        stbrp_rect rect;
        rect.id = codepoint;
        rect.w = w + PADDING * 2 + shadowPadding;
        rect.h = h + PADDING * 2 + shadowPadding;
        stbrp_pack_rects(m_pRectContext, &rect, 1);

        if (!rect.was_packed)
        {
            CORE_ERROR("Failed to pack character %c into font.", (char)codepoint);
            return false;
        }
        
        // render character (stride and offset is important here)
        //int byteOffset = (rect.x + PADDING) + ((rect.y + PADDING) * ATLAS_SIZE);
        //stbtt_MakeCodepointBitmap(m_pInfo, m_pAtlasData + byteOffset, w, h, ATLAS_SIZE, m_scale, m_scale, codepoint);
        static std::vector<uint8_t> charData;
        charData.resize(w * h);
        stbtt_MakeCodepointBitmap(m_pInfo, charData.data(), w, h, w, m_scale, m_scale, codepoint);

        auto dstW = w + shadowPadding;
        auto dstH = h + shadowPadding;

        for (int srcy = 0; srcy < dstH; ++srcy)
        {
            int dsty = rect.y + PADDING + srcy;
            for (int srcx = 0; srcx < dstW; ++srcx)
            {
                uint8_t c = 0;
                int dstx = rect.x + PADDING + srcx;
                int dstk = ((dsty * ATLAS_SIZE) + dstx) * 4;

                if (srcx < w && srcy < h)
                {
                    int srck = srcy * w + srcx;
                    c = charData[srck];
                }

                if (shadowAlpha && srcx > m_shadowDistance && srcy > m_shadowDistance)
                {
                    int shadowk = (srcy - m_shadowDistance) * w + (srcx - m_shadowDistance);
                    uint8_t s = charData[shadowk];

                    m_pAtlasData[dstk + 0] = c;
                    m_pAtlasData[dstk + 1] = c;
                    m_pAtlasData[dstk + 2] = c;
                    m_pAtlasData[dstk + 3] = std::max(s, c);
                }
                else
                {
                    m_pAtlasData[dstk + 0] = c;
                    m_pAtlasData[dstk + 1] = c;
                    m_pAtlasData[dstk + 2] = c;
                    m_pAtlasData[dstk + 3] = c;
                }
            }
        }

        // Create the character
        Char* chr = new Char();
        chr->xAdvance = (float)ax * m_scale;
        chr->uvs = {
            (float)(rect.x + PADDING) / (float)ATLAS_SIZE,
            (float)(rect.y + PADDING) / (float)ATLAS_SIZE,
            (float)(rect.x + rect.w - PADDING) / (float)ATLAS_SIZE,
            (float)(rect.y + rect.h - PADDING) / (float)ATLAS_SIZE
        };
        chr->offset = {
            (float)lsb * m_scale,
            (float)m_ascent + (float)c_y1
        };
        m_chars[codepoint] = chr;

        return true;
    }

    void Font::updateAtlas()
    {
        m_pAtlas->setData(m_pAtlasData);
    }

    glm::vec2 Font::measure(const std::string& text)
    {
        glm::vec2 size{0.0f, (float)m_height};
        bool atlasModified = false;

        // Naive approach for now, only ASCII characters (English)
        // TODO: UTF8 (Maybe we won't need it)
        float x = 0.0f;
        for (int i = 0, len = (int)text.length(); i < len; ++i)
        {
            auto c = text[i];

            // If return character, start a new line
            if (c == '\n')
            {
                size.y += (float)m_height;
                size.x = std::max(size.x, x);
                x = 0.0f;
                continue;
            }

            auto it = m_chars.find((int)c);
            if (it == m_chars.end())
            {
                if (!addGlyph((int)c)) continue;
                atlasModified = true;
                it = m_chars.find((int)c);
            }
            const auto& chr = it->second;

            // Advance
            float xAdvance = chr->xAdvance;

            // Kerning
            if (i + 1 < len)
            {
                auto nextC = text[i + 1];
                int kern = stbtt_GetCodepointKernAdvance(m_pInfo, (int)c, (int)nextC);
                xAdvance += (float)kern * m_scale;
            }

            x += xAdvance;
        }

        if (atlasModified) updateAtlas();

        size.x = std::max(size.x, x);
        return size;
    }
    
    //void Font::draw(const std::string& text,
    //                const glm::mat4& transform, 
    //                const glm::vec4& color,
    //                float scale,
    //                const glm::vec2& align)
    //{
    //    // no op
    //}

    void Font::draw(const std::string& text,
                    const glm::vec2& position, 
                    const glm::vec4& color,
                    float rotation,
                    float scale,
                    const glm::vec2& align)
    {
        auto sb = Engine::getSpriteBatch().get();

        bool atlasModified = false;

        auto size = measure(text);
        auto hSize = size * 0.5f;

        auto radTheta = glm::radians(rotation);
        auto sinTheta = std::sin(radTheta);
        auto cosTheta = std::cos(radTheta);
        glm::vec2 right{cosTheta, sinTheta};
        glm::vec2 down{-sinTheta, cosTheta};
        auto downN = glm::normalize(down);

        // Top left
        glm::vec2 topLeft = position - (right * size.x * align.x * scale) - (down * size.y * align.y * scale);
        glm::vec2 pos = topLeft;
        int line = 0;

        // Naive approach for now, only ASCII characters (English)
        // TODO: UTF8 (Maybe we won't need it)
        for (int i = 0, len = (int)text.length(); i < len; ++i)
        {
            auto c = text[i];

            // If return character, start a new line
            if (c == '\n')
            {
                ++line;
                pos = topLeft + downN * (float)line * (float)m_height * scale;
                continue;
            }

            auto it = m_chars.find((int)c);
            if (it == m_chars.end())
            {
                if (!addGlyph((int)c)) continue;
                atlasModified = true;
                it = m_chars.find((int)c);
            }
            const auto& chr = it->second;

            glm::vec2 offset = 
                chr->offset.x * right * scale + 
                chr->offset.y * down * scale;
            sb->drawSprite(m_pAtlas, pos + offset, color, rotation, glm::vec2(scale), {0.0f, 0.0f}, chr->uvs);

            // Advance
            float xAdvance = chr->xAdvance;

            // Kerning
            if (i + 1 < len)
            {
                auto nextC = text[i + 1];
                int kern = stbtt_GetCodepointKernAdvance(m_pInfo, (int)c, (int)nextC);
                xAdvance += (float)kern * m_scale;
            }

            pos += right * (xAdvance * scale);
        }

        if (atlasModified) updateAtlas();
    }
}
