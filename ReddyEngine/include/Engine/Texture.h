#pragma once

#include <glm/vec2.hpp>
#include <SDL_opengl.h>

#include <memory>
#include <string>


namespace Engine
{
    class Texture;
    using TextureRef = std::shared_ptr<Texture>;


    class Texture
    {
    public:
        // Data is interleaved RGBA
        static TextureRef createFromData(const glm::ivec2& size, const uint8_t* data);
        static TextureRef createFromFile(const std::string& filename);

        const glm::ivec2& getSize() const { return m_size; }
        const std::string& getName() const { return m_name; }

        void bind(int slot = 0);

    private:
        Texture();

        glm::ivec2 m_size = { 0, 0 };
        GLuint m_handle = 0;
        std::string m_name;
    };
}
