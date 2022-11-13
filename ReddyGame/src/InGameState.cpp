extern "C" {
	#include <../lua/lua.h>
	#include <../lua/lauxlib.h>
	#include <../lua/lualib.h>
}

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
#include <Engine/SpriteBatch.h>
#include <Engine/LuaBindings.h>

#include <glm/gtx/transform.hpp>


InGameState::InGameState(const std::string& filename)
    : GameState(filename)
{
}

void InGameState::enter(const GameStateRef& previousState)
{
    GameState::enter(previousState);

    REGISTER_EVENT(KeyDownEvent, InGameState::onKeyDown);
	Engine::getInput()->setMouseCursor("assets/textures/cursor.png", glm::ivec2(4, 0));
}


void InGameState::leave(const GameStateRef& newState)
{
    DEREGISTER_EVENT(KeyDownEvent);

    GameState::leave(newState);
}

void InGameState::onKeyDown(Engine::IEvent* pEvent)
{
    auto keyEvent = (Engine::KeyDownEvent*)pEvent;
    if (keyEvent->key.keysym.scancode == SDL_SCANCODE_ESCAPE)
        std::dynamic_pointer_cast<Game>(Engine::getGame())->changeState(Engine::StateChangeRequest::MainMenu, "");
}
