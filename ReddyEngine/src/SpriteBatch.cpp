#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <backends/imgui_impl_opengl3_loader.h>

#include "Engine/SpriteBatch.h"
#include "Engine/Log.h"
#include "Engine/Texture.h"

#include <glm/glm.hpp>
#include <imgui.h>
#include <SDL_opengl_glext.h>


static const int MAX_SPRITE_COUNT = 300; // 1200 vertices

static const GLchar* VERTEX_SHADER =
    "uniform mat4 ProjMtx;\n"
    "uniform mat4 ViewMtx;\n"
    "in vec2 Position;\n"
    "in vec2 UV;\n"
    "in vec4 Color;\n"
    "out vec2 Frag_UV;\n"
    "out vec4 Frag_Color;\n"
    "void main()\n"
    "{\n"
    "    Frag_UV = UV;\n"
    "    Frag_Color = Color;\n"
    "    gl_Position = ProjMtx * ViewMtx * vec4(Position.xy,0,1);\n"
    "}\n";

static const GLchar* FRAGMENT_SHADER =
    "uniform sampler2D Texture;\n"
    "in vec2 Frag_UV;\n"
    "in vec4 Frag_Color;\n"
    "out vec4 Out_Color;\n"
    "void main()\n"
    "{\n"
    "    Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
    "}\n";



static bool CheckShader(GLuint handle, const char* desc)
{
    GLint status = 0, log_length = 0;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
    glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &log_length);

    if ((GLboolean)status == GL_FALSE)
    {
        CORE_FATAL("ERROR: SpriteBatch: failed to compile {}! With GLSL: {}\n", desc, "#version 130");
    }

    if (log_length > 1)
    {
        char* buf = new char[log_length + 1];
        glGetShaderInfoLog(handle, log_length, nullptr, (GLchar*)buf);
        CORE_FATAL("{}\n", buf);
        assert(false);
    }

    return (GLboolean)status == GL_TRUE;
}

static bool CheckProgram(GLuint handle, const char* desc)
{
    GLint status = 0, log_length = 0;
    glGetProgramiv(handle, GL_LINK_STATUS, &status);
    glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &log_length);

    if ((GLboolean)status == GL_FALSE)
    {
        CORE_FATAL("ERROR: SpriteBatch: failed to link {}! With GLSL {}\n", desc, "#version 130");
    }

    if (log_length > 1)
    {
        char* buf = new char[log_length + 1];
        glGetShaderInfoLog(handle, log_length, nullptr, (GLchar*)buf);
        CORE_FATAL("{}\n", buf);
        assert(false);
    }

    return (GLboolean)status == GL_TRUE;
}


namespace Engine
{
    SpriteBatch::SpriteBatch()
    {
        m_vertices = new Vertex[MAX_SPRITE_COUNT * 4];

        // Create shaders
        const GLchar* vertex_shader_with_version[2] = { "#version 130\n", VERTEX_SHADER };
        GLuint vert_handle = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vert_handle, 2, vertex_shader_with_version, nullptr);
        glCompileShader(vert_handle);
        CheckShader(vert_handle, "vertex shader");

        const GLchar* fragment_shader_with_version[2] = { "#version 130\n", FRAGMENT_SHADER };
        GLuint frag_handle = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(frag_handle, 2, fragment_shader_with_version, nullptr);
        glCompileShader(frag_handle);
        CheckShader(frag_handle, "fragment shader");
        
        // Link
        m_shader = glCreateProgram();
        glAttachShader(m_shader, vert_handle);
        glAttachShader(m_shader, frag_handle);
        glLinkProgram(m_shader);
        CheckProgram(m_shader, "shader program");

        glDetachShader(m_shader, vert_handle);
        glDetachShader(m_shader, frag_handle);
        glDeleteShader(vert_handle);
        glDeleteShader(frag_handle);

        m_attribLocationTexture = glGetUniformLocation(m_shader, "Texture");
        m_attribLocationProj = glGetUniformLocation(m_shader, "ProjMtx");
        m_attribLocationView = glGetUniformLocation(m_shader, "ViewMtx");
        m_attribLocationVertexPos = (GLuint)glGetAttribLocation(m_shader, "Position");
        m_attribLocationVertexTexCoord = (GLuint)glGetAttribLocation(m_shader, "UV");
        m_attribLocationVertexColor = (GLuint)glGetAttribLocation(m_shader, "Color");

        // Create buffers
        glGenBuffers(1, &m_vbo);
        glGenBuffers(1, &m_elements);

        // Create default white texture to use instead if no texture is passed
        uint32_t white = 0xFFFFFFFF;
        m_pDefaultWhiteTexture = Texture::createFromData({ 1, 1 }, (uint8_t*)&white);

        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * MAX_SPRITE_COUNT * 4, nullptr, GL_STREAM_DRAW);

        // Upload indices ahead of time
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elements);
        uint16_t indices[MAX_SPRITE_COUNT * 6];
        for (unsigned int i = 0; i < MAX_SPRITE_COUNT; ++i)
        {
            indices[i * 6 + 0] = i * 4 + 0;
            indices[i * 6 + 1] = i * 4 + 1;
            indices[i * 6 + 2] = i * 4 + 2;
            indices[i * 6 + 3] = i * 4 + 2;
            indices[i * 6 + 4] = i * 4 + 3;
            indices[i * 6 + 5] = i * 4 + 0;
        }
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * MAX_SPRITE_COUNT * 6, indices, GL_STATIC_DRAW);
    }

    SpriteBatch::~SpriteBatch()
    {
        delete[] m_vertices;
    }

    void SpriteBatch::beginFrame()
    {
        const auto& io = ImGui::GetIO();

        glUseProgram(m_shader);
        glUniform1i(m_attribLocationTexture, 0);
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elements);
        glEnableVertexAttribArray(m_attribLocationVertexPos);
        glEnableVertexAttribArray(m_attribLocationVertexTexCoord);
        glEnableVertexAttribArray(m_attribLocationVertexColor);
        glVertexAttribPointer(m_attribLocationVertexPos, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
        glVertexAttribPointer(m_attribLocationVertexTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoord));
        glVertexAttribPointer(m_attribLocationVertexColor, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // Premultiplied
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_SCISSOR_TEST); // Enable that for future
        
        float L = 0;
        float R = io.DisplaySize.x;
        float T = 0;
        float B = io.DisplaySize.y;
        const float ortho_projection[4][4] =
        {
            { 2.0f/(R-L),   0.0f,         0.0f,   0.0f },
            { 0.0f,         2.0f/(T-B),   0.0f,   0.0f },
            { 0.0f,         0.0f,        -1.0f,   0.0f },
            { (R+L)/(L-R),  (T+B)/(B-T),  0.0f,   1.0f },
        };
        glUniformMatrix4fv(m_attribLocationProj, 1, GL_FALSE, &ortho_projection[0][0]);
    }

    void SpriteBatch::begin(const glm::mat4& transform)
    {
        CORE_ASSERT(!m_isInBatch, "SpriteBatch::begin() called without previously called end() on a previous batch");

        m_isInBatch = true;
        m_transform = transform;

        glUniformMatrix4fv(m_attribLocationView, 1, GL_FALSE, &transform[0][0]);
    }

    void SpriteBatch::end()
    {
        CORE_ASSERT(m_isInBatch, "SpriteBatch::end() called without calling begin() first");

        flush();
        m_isInBatch = false;
    }

    // Long ass method that will be used everywhere by everything
    void SpriteBatch::drawSprite(const TextureRef& pTexture,
                                 const glm::vec2& position, 
                                 const glm::vec4& color, 
                                 float rotation, 
                                 const glm::vec2& scale, 
                                 const glm::vec2& origin, 
                                 const glm::vec4& uvs)
    {
        CORE_ASSERT(m_isInBatch, "SpriteBatch::draw() called without calling begin() first");

        if (m_pCurrentTexture != pTexture)
        {
            flush();
            m_pCurrentTexture = pTexture;
        }

        glm::ivec2 textureSize = m_pCurrentTexture ? m_pCurrentTexture->getSize() : glm::ivec2{ 1, 1 };
        auto sizexf = static_cast<float>(textureSize.x);
        auto sizeyf = static_cast<float>(textureSize.y);
        sizexf *= std::abs(uvs.z - uvs.x);
        sizeyf *= std::abs(uvs.w - uvs.y);
        auto hSize = glm::vec2(sizexf * .5f * scale.x, sizeyf * .5f * scale.y);
        auto radTheta = glm::radians(rotation);
        auto sinTheta = std::sin(radTheta);
        auto cosTheta = std::cos(radTheta);
        auto invOrigin = glm::vec2(1.f - origin.x, 1.f - origin.y) * 2.f;

        glm::vec2 right{cosTheta * hSize.x, sinTheta * hSize.x};
        glm::vec2 down{-sinTheta * hSize.y, cosTheta * hSize.y};

        Vertex* pVerts = m_vertices + (m_spriteCount * 4);
        pVerts[0].position = position;
        pVerts[0].position -= right * origin.x * 2.f;
        pVerts[0].position -= down * origin.y * 2.f;
        pVerts[0].texCoord = {uvs.x, uvs.y};
        pVerts[0].color = color;

        pVerts[1].position = position;
        pVerts[1].position -= right * origin.x * 2.f;
        pVerts[1].position += down * invOrigin.y;
        pVerts[1].texCoord = {uvs.x, uvs.w};
        pVerts[1].color = color;

        pVerts[2].position = position;
        pVerts[2].position += right * invOrigin.x;
        pVerts[2].position += down * invOrigin.y;
        pVerts[2].texCoord = {uvs.z, uvs.w};
        pVerts[2].color = color;

        pVerts[3].position = position;
        pVerts[3].position += right * invOrigin.x;
        pVerts[3].position -= down * origin.y * 2.f;
        pVerts[3].texCoord = {uvs.z, uvs.y};
        pVerts[3].color = color;

        ++m_spriteCount;

        if (m_spriteCount == MAX_SPRITE_COUNT)
        {
            flush();
        }
    }

    void SpriteBatch::drawSprite(const TextureRef& pTexture, // nullptr for 1x1 white
                                 const glm::mat4& transform, 
                                 const glm::vec4& color, 
                                 const glm::vec2& scale, 
                                 const glm::vec2& origin, 
                                 const glm::vec4& uvs)
    {
        CORE_ASSERT(m_isInBatch, "SpriteBatch::draw() called without calling begin() first");

        if (!scale.x || !scale.y) return; // Scale 0, can't draw this

        if (m_pCurrentTexture != pTexture)
        {
            flush();
            m_pCurrentTexture = pTexture;
        }

        glm::ivec2 textureSize = m_pCurrentTexture ? m_pCurrentTexture->getSize() : glm::ivec2{ 1, 1 };
        glm::vec2 sizef = glm::vec2((float)textureSize.x, (float)textureSize.y) * scale;
        glm::vec2 invOrigin(1.f - origin.x, 1.f - origin.y);

        Vertex* pVerts = m_vertices + (m_spriteCount * 4);
        pVerts[0].position = transform * glm::vec4(-sizef.x * origin.x, -sizef.y * origin.y, 0, 1);
        pVerts[0].texCoord = {uvs.x, uvs.y};
        pVerts[0].color = color;

        pVerts[1].position = transform * glm::vec4(-sizef.x * origin.x, sizef.y * origin.y, 0, 1);
        pVerts[1].texCoord = {uvs.x, uvs.w};
        pVerts[1].color = color;

        pVerts[2].position = transform * glm::vec4(sizef.x * origin.x, sizef.y * origin.y, 0, 1);
        pVerts[2].texCoord = {uvs.z, uvs.w};
        pVerts[2].color = color;

        pVerts[3].position = transform * glm::vec4(sizef.x * origin.x, -sizef.y * origin.y, 0, 1);
        pVerts[3].texCoord = {uvs.z, uvs.y};
        pVerts[3].color = color;

        ++m_spriteCount;

        if (m_spriteCount == MAX_SPRITE_COUNT)
        {
            flush();
        }
    }

    void SpriteBatch::drawLine(const glm::vec2& from, const glm::vec2& to, float size, const glm::vec4& color)
    {
        // We're cheating here. I wanted to use GL_LINES, but our imgui wrangler doesn't support them? .. So I gave up instead of spending hours re-setting up OpenGL.
        auto mid = (from + to) * 0.5f;
        auto dir = to - from;
        auto len = glm::length(dir);
        if (len == 0.0f) return;
        dir /= len;
        auto angle = glm::degrees(std::atan2f(dir.y, dir.x));

        drawSprite(m_pDefaultWhiteTexture, mid, color, angle, {len, size});
    }

    void SpriteBatch::drawRect(const TextureRef& pTexture,
                               const glm::vec4& rect,
                               const glm::vec4& color,
                               const glm::vec4& uvs)
    {
        CORE_ASSERT(m_isInBatch, "SpriteBatch::drawRect() called without calling begin() first");

        if (m_pCurrentTexture != pTexture)
        {
            flush();
            m_pCurrentTexture = pTexture;
        }

        Vertex* pVerts = m_vertices + (m_spriteCount * 4);
        pVerts[0].position = {rect.x, rect.y};
        pVerts[0].texCoord = {uvs.x, uvs.y};
        pVerts[0].color = color;

        pVerts[1].position = {rect.x, rect.y + rect.w};
        pVerts[1].texCoord = {uvs.x, uvs.w};
        pVerts[1].color = color;

        pVerts[2].position = {rect.x + rect.z, rect.y + rect.w};
        pVerts[2].texCoord = {uvs.z, uvs.w};
        pVerts[2].color = color;

        pVerts[3].position = {rect.x + rect.z, rect.y};
        pVerts[3].texCoord = {uvs.z, uvs.y};
        pVerts[3].color = color;

        ++m_spriteCount;

        if (m_spriteCount == MAX_SPRITE_COUNT)
        {
            flush();
        }
    }

    void SpriteBatch::flush()
    {
        if (!m_spriteCount) return;
        if (!m_pCurrentTexture) m_pCurrentTexture = m_pDefaultWhiteTexture;
        m_pCurrentTexture->bind();

        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * m_spriteCount * 4, (const GLvoid*)m_vertices);
        glDrawElements(GL_TRIANGLES, (GLsizei)(m_spriteCount * 6), GL_UNSIGNED_SHORT, 0);

        m_spriteCount = 0;
        m_pCurrentTexture = nullptr;
    }
}
