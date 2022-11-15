#include "GameState.h"
#include "Game.h"
#include "MainMenuState.h"

#include <Engine/ReddyEngine.h>
#include <Engine/EventSystem.h>
#include <Engine/SpriteBatch.h>
#include <Engine/Scene.h>
#include <Engine/LuaBindings.h>
#include <Engine/Utils.h>
#include <Engine/Log.h>
#include <Engine/Input.h>

#include <glm/gtx/transform.hpp>


GameState::GameState(const std::string& filename)
    : m_filenameToLoad(filename)
{
}

void GameState::enter(const GameStateRef& previousState)
{
    Engine::getLuaBindings()->init(false);

    auto L = Engine::getLuaBindings()->getState();

    Engine::getLuaBindings()->runFiles();
    Engine::getLuaBindings()->initComponents();

    // Load the game!
    Json::Value json;
    if (!Engine::Utils::loadJson(json, m_filenameToLoad))
    {
        CORE_ERROR_POPUP("Failed to load scene: {}", m_filenameToLoad);
        std::dynamic_pointer_cast<Game>(Engine::getGame())->changeState(std::make_shared<MainMenuState>());
        return;
    }
    Engine::getScene()->deserialize(json);
}

void GameState::leave(const GameStateRef& newState)
{
    Engine::getScene()->clear();
    Engine::getLuaBindings()->clear();
}

void GameState::fixedUpdate(float dt)
{
    Engine::getEventSystem()->dispatchEvents();
    Engine::getScene()->fixedUpdate(dt);
}

void GameState::update(float dt)
{
    Engine::getEventSystem()->dispatchEvents();

    auto ratio = Engine::getResolution().y / 1440.0f;
    auto mousePos = Engine::getInput()->getMousePos();
    mousePos -= Engine::getResolution() * 0.5f;
    mousePos /= zoom * ratio;
    auto mouseWorldPosition = mousePos + camera;
    Engine::getScene()->setMousePos(mouseWorldPosition);

    Engine::getScene()->update(dt);
}

void GameState::draw()
{
    auto ratio = Engine::getResolution().y / 1440.0f;
    glm::mat4 transform = 
        glm::translate(glm::vec3(Engine::getResolution() * 0.5f, 0.0f)) *
        glm::scale(glm::vec3(zoom * ratio)) * 
        glm::translate(glm::vec3(-camera, 0.0f));

    auto sb = Engine::getSpriteBatch().get();
    sb->begin(transform);

    Engine::getScene()->draw();

    sb->end();
}
