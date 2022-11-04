#include "Game.h"

#include <Engine/ReddyEngine.h>
#include <Engine/SpriteBatch.h>
#include <Engine/Texture.h>

#include <imgui.h>


static float rotation = 0.0f;
static Engine::TextureRef pTexture;


void Game::loadContent()
{
    pTexture = Engine::Texture::createFromFile("assets/textures/5960_NotLikeThis.png");
}


void Game::update(float deltatime)
{
    rotation += 90.0f * deltatime;
}


void Game::fixedUpdate(float deltatime)
{
}


void Game::draw()
{
    auto sb = Engine::getSpriteBatch();
    auto res = Engine::getResolution();

    sb->begin();
    sb->drawRect(nullptr, {0, 0, 100, 100}, {1, 0, 0, 1});
    sb->draw(pTexture, glm::vec2((float)res.x, (float)res.y) * 0.5f, glm::vec4(1, 1, 1, 1), rotation);
    sb->end();
}
