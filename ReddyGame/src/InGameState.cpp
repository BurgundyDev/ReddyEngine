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

static InGameState* g_pInGameState = nullptr; // Global reference for Lua bindings

#define LUA_REGISTER(name) lua_register(L, #name, [](lua_State* L){return g_pInGameState->func ## name(L);})


InGameState::InGameState(const std::string& filename)
    : m_filenameToLoad(filename)
{
}

void InGameState::enter(const GameStateRef& previousState)
{
    g_pInGameState = this;

    Engine::getLuaBindings()->clear();
    Engine::getLuaBindings()->init(false);

    auto L = Engine::getLuaBindings()->getState();

    LUA_REGISTER(GetCameraPosition);
    LUA_REGISTER(SetCameraPosition);
    LUA_REGISTER(GetCameraZoom);
    LUA_REGISTER(SetCameraZoom);

    Engine::getLuaBindings()->runFiles();
    Engine::getLuaBindings()->initComponents();

    REGISTER_EVENT(KeyDownEvent, InGameState::onKeyDown);

	Engine::getInput()->setMouseCursor("assets/textures/cursor.png", glm::ivec2(4, 0));

    // Load the game!
    Json::Value json;
    if (!Engine::Utils::loadJson(json, m_filenameToLoad))
    {
        CORE_ERROR_POPUP("Failed to load level: {}", m_filenameToLoad);
        std::dynamic_pointer_cast<Game>(Engine::getGame())->changeState(std::make_shared<MainMenuState>());
        return;
    }
    Engine::getScene()->deserialize(json);
}


void InGameState::leave(const GameStateRef& newState)
{
    Engine::getScene()->clear();

    DEREGISTER_EVENT(KeyDownEvent);

    g_pInGameState = nullptr;
}

void InGameState::onKeyDown(Engine::IEvent* pEvent)
{
    auto keyEvent = (Engine::KeyDownEvent*)pEvent;
    if (keyEvent->key.keysym.scancode == SDL_SCANCODE_ESCAPE)
        std::dynamic_pointer_cast<Game>(Engine::getGame())->pushState(std::make_shared<InGameMenuState>());
}

void InGameState::fixedUpdate(float dt)
{
    Engine::getEventSystem()->dispatchEvents();
    Engine::getScene()->fixedUpdate(dt);
}

void InGameState::update(float dt)
{
    Engine::getEventSystem()->dispatchEvents();

    auto mousePos = Engine::getInput()->getMousePos();
    mousePos -= Engine::getResolution() * 0.5f;
    mousePos /= m_zoom;
    m_mouseWorldPos = mousePos + m_camera;
    Engine::getScene()->setMousePos(m_mouseWorldPos);

    Engine::getScene()->update(dt);
}

void InGameState::draw()
{
    glm::mat4 transform = 
        glm::translate(glm::vec3(Engine::getResolution() * 0.5f, 0.0f)) *
        glm::scale(glm::vec3(m_zoom)) * 
        glm::translate(glm::vec3(-m_camera, 0.0f));

    auto sb = Engine::getSpriteBatch().get();
    sb->begin(transform);

    Engine::getScene()->draw();

    sb->end();
}

int InGameState::funcGetCameraPosition(lua_State* L)
{
    LUA_PUSH_VEC2(m_camera);
    return 1;
}

int InGameState::funcSetCameraPosition(lua_State* L)
{
    m_camera = LUA_GET_VEC2(1, glm::vec2(0));
    return 0;
}

int InGameState::funcGetCameraZoom(lua_State* L)
{
    lua_pushnumber(L, (lua_Number)m_zoom);
    return 1;
}

int InGameState::funcSetCameraZoom(lua_State* L)
{
    m_zoom = (float)lua_tonumber(L, -1);
    return 0;
}
