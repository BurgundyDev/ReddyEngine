#pragma once

#include "GameState.h"

#include <glm/vec2.hpp>

#include <string>


namespace Engine
{
    class IEvent;
};

extern "C" {
    struct lua_State;
}


class InGameState final : public GameState
{
public:
    InGameState(const std::string& filename);

    void enter(const GameStateRef& previousState) override;
    void leave(const GameStateRef& newState) override;
    void update(float dt) override;
    void fixedUpdate(float dt) override;
    void draw() override;

    int funcGetCameraPosition(lua_State* L);
    int funcSetCameraPosition(lua_State* L);
    int funcGetCameraZoom(lua_State* L);
    int funcSetCameraZoom(lua_State* L);

private:
    void onKeyDown(Engine::IEvent* pEvent);

    bool m_failure = false;
    std::string m_filenameToLoad;
    glm::vec2 m_camera = {0,0};
    glm::vec2 m_cameraTarget = {0,0};
    float m_zoom = 128.0f;
    float m_zoomTarget = 128.0f;
    glm::vec2 m_mouseWorldPos;
};
