extern "C" {
	#include <../lua/lua.h>
	#include <../lua/lauxlib.h>
	#include <../lua/lualib.h>
}

#include "InGameState.h"
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
#include <Engine/Entity.h>

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

    Engine::Utils::loadJson(m_inGameMenuJson, "assets/scenes/inGameMenu.json");
}

void InGameState::leave(const GameStateRef& newState)
{
    DEREGISTER_EVENT(KeyDownEvent);

    GameState::leave(newState);
}

void InGameState::showInGameMenu()
{
    m_subState = SubState::InGameMenu;

    auto pWorld = Engine::getScene()->getEntityByName("World", true);
    pWorld->disable();

    m_pInGameMenu = Engine::getScene()->createEntityFromJson(Engine::getScene()->getRoot(), m_inGameMenuJson["root"], true);
}

void InGameState::hideInGameMenu()
{
    m_subState = SubState::InGame;

    Engine::getScene()->destroyEntity(m_pInGameMenu);
    m_pInGameMenu = nullptr;

    auto pWorld = Engine::getScene()->getEntityByName("World", true);
    pWorld->enable();
}

void InGameState::onKeyDown(Engine::IEvent* pEvent)
{
    auto keyEvent = (Engine::KeyDownEvent*)pEvent;
    if (keyEvent->key.keysym.scancode == SDL_SCANCODE_ESCAPE)
    {
        switch (m_subState)
        {
            case SubState::InGame:
                showInGameMenu();
                break;
            case SubState::InGameMenu:
                hideInGameMenu();
                break;
        }
    }
}
