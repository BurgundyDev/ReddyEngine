#pragma once

#include <glm/vec2.hpp>
#include <SDL_opengl.h>

#include <memory>
#include <string>

#include "Resource.h"


namespace Engine
{
    class Texture;
    using TextureRef = std::shared_ptr<Texture>;


    enum class TextureFormat
    {
        R8G8B8A8,
        R8
    };


    class Texture : public Resource
    {
    public:
        // Data is interleaved RGBA
        static TextureRef createFromData(const glm::ivec2& size, const uint8_t* pData);
        static TextureRef createFromFile(const std::string& filename);
        static TextureRef createDynamic(const glm::ivec2& size, TextureFormat format = TextureFormat::R8G8B8A8);

        void setData(const uint8_t* data); // For dynamic textures only

        const glm::ivec2& getSize() const { return m_size; }

        void bind(int slot = 0);

    private:
        Texture();

        glm::ivec2 m_size = { 0, 0 };
        GLuint m_handle = 0;
        bool m_isDynamic = false;
        TextureFormat m_format = TextureFormat::R8G8B8A8;
    };
}
