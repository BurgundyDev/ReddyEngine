#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <SDL_opengl.h>

#include <memory>


namespace Engine
{
    class Texture;
    using TextureRef = std::shared_ptr<Texture>;


    class SpriteBatch final
    {
    public:
        SpriteBatch();
        ~SpriteBatch();

        void beginFrame(); // Called once per frame
        void begin(const glm::mat4& transform = glm::mat4(1));
        void end(); // This will draw if any sprites are pending

        // Long ass method that will be used everywhere by everything
        void draw(const TextureRef& pTexture, // nullptr for 1x1 white
                  const glm::vec2& position, 
                  const glm::vec4& color = { 1, 1, 1, 1 }, 
                  float rotation = 0.0f, 
                  float scale = 1.0f, 
                  const glm::vec2& origin = { 0.5f, 0.5f }, 
                  const glm::vec4& uvs = { 0, 0, 1, 1 });

        void drawRect(const TextureRef& pTexture, // nullptr for white
                      const glm::vec4& rect, // x, y, w, h
                      const glm::vec4& color = { 1, 1, 1, 1 }, 
                      const glm::vec4& uvs = { 0, 0, 1, 1 }); // u1, v1, u2, v2

        // Force draw pending sprites
        void flush();

        const glm::mat4& getTransform() const { return m_transform; }

    private:
        struct Vertex
        {
            glm::vec2 position;
            glm::vec2 texCoord;
            glm::vec4 color;
        };

        bool m_isInBatch = false;
        TextureRef m_pCurrentTexture;
        int m_spriteCount = 0;
        Vertex* m_vertices = nullptr;
        TextureRef m_pDefaultWhiteTexture;
        glm::mat4 m_transform;

        GLuint m_attribLocationProj = 0;
        GLuint m_attribLocationView = 0;
        GLuint m_attribLocationTexture = 0;
        GLuint m_attribLocationVertexPos = 0;
        GLuint m_attribLocationVertexTexCoord = 0;
        GLuint m_attribLocationVertexColor = 0;
        GLuint m_shader = 0;
        GLuint m_vao = 0;
        unsigned int m_vbo;
        unsigned int m_elements;
    };
}
