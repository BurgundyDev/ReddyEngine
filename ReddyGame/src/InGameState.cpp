#include "InGameState.h"
#include "InGameMenuState.h"
#include "MainMenuState.h"
#include "Game.h"

#include <Engine/Input.h>
#include <Engine/ReddyEngine.h>
#include <Engine/Scene.h>
#include <Engine/EventSystem.h>
#include <Engine/Utils.h>
#include <Engine/Log.h>


InGameState::InGameState(const std::string& filename)
    : m_filenameToLoad(filename)
{
}

void InGameState::enter(const GameStateRef& previousState)
{
    REGISTER_EVENT(KeyDownEvent, InGameState::onKeyDown);

	Engine::getInput()->setMouseCursor("assets/textures/cursor.png", glm::ivec2(4, 0));

    // Load the game!
    Json::Value json;
    if (!Engine::Utils::loadJson(json, m_filenameToLoad))
    {
        CORE_ERROR_POPUP("Failed to load level: {}", m_filenameToLoad);
        g_pGame->changeState(std::make_shared<MainMenuState>());
        return;
    }
    Engine::getScene()->deserialize(json);
}

void InGameState::leave(const GameStateRef& newState)
{
    DEREGISTER_EVENT(KeyDownEvent);
}

void InGameState::onKeyDown(Engine::IEvent* pEvent)
{
    auto keyEvent = (Engine::KeyDownEvent*)pEvent;
    if (keyEvent->key.keysym.scancode == SDL_SCANCODE_ESCAPE)
        g_pGame->pushState(std::make_shared<InGameMenuState>());
}
