#include "Engine/Texture.h"
#include "Engine/Log.h"
#include "Engine/Utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


namespace Engine
{
    // Data is interleaved RGBA
    TextureRef Texture::createFromData(const glm::ivec2& size, const uint8_t* data)
    {
        auto pRet = std::shared_ptr<Texture>(new Texture());

        pRet->m_size = size;

        glGenTextures(1, &pRet->m_handle);
        glBindTexture(GL_TEXTURE_2D, pRet->m_handle);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Hum not a lot of thing will repeat in this game. But this should be set from sprite batch anyway
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        return pRet;
    }

    TextureRef Texture::createFromFile(const std::string& filename)
    {
        int w, h, n;
        auto image = stbi_load(filename.c_str(), &w, &h, &n, 4);
        if (!image)
        {
            CORE_ERROR("Failed to load texture: {}", filename);
            return nullptr;
        }
        glm::ivec2 size{ w, h };

        // Pre multiplied
        uint8_t* pImageData = &(image[0]);
        auto len = size.x * size.y;
        for (decltype(len) i = 0; i < len; ++i, pImageData += 4)
        {
            pImageData[0] = pImageData[0] * pImageData[3] / 255;
            pImageData[1] = pImageData[1] * pImageData[3] / 255;
            pImageData[2] = pImageData[2] * pImageData[3] / 255;
        }

        auto pRet = createFromData(size, image);
        stbi_image_free(image);
        return pRet;
    }

    // In OpenGL this doesn't change much, but we plan ahead in case we go DX where it will matter.
    TextureRef Texture::createDynamic(const glm::ivec2& size, TextureFormat format)
    {
        auto pRet = std::shared_ptr<Texture>(new Texture());
        pRet->m_format = format;
        
        GLuint handle;
        glGenTextures(1, &handle);
        glBindTexture(GL_TEXTURE_2D, handle);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        pRet->m_isDynamic = true;
        pRet->m_size = size;
        pRet->m_handle = handle;
        
        return pRet;
    }

    Texture::Texture() {}
    
    void Texture::bind(int slot)
    {
        // Ignore slot for now until we have more complex shaders
        glBindTexture(GL_TEXTURE_2D, m_handle);
    }

    void Texture::setData(const uint8_t* pData)
    {
        CORE_FATAL(m_isDynamic, "Attempt to set data on a static texture. Use ::createDynamic()");

        glBindTexture(GL_TEXTURE_2D, m_handle);

        GLint internalFormat = GL_RGBA;
        GLenum format = GL_RGBA;

        switch (m_format)
        {
            case TextureFormat::R8G8B8A8:
                internalFormat = GL_RGBA;
                format = GL_RGBA;
                break;
            case TextureFormat::R8:
                internalFormat = GL_LUMINANCE;
                format = GL_RED;
                break;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_size.x, m_size.y, 0, format, GL_UNSIGNED_BYTE, pData);
    }
}
