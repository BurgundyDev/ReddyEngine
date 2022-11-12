#pragma once

#include "GameState.h"

#include <glm/vec2.hpp>

#include <string>


namespace Engine
{
    class IEvent;
};


class InGameState final : public GameState
{
public:
    InGameState(const std::string& filename);

    void enter(const GameStateRef& previousState) override;
    void leave(const GameStateRef& newState) override;
    void update(float dt) override;
    void fixedUpdate(float dt) override;
    void draw() override;

private:
    void onKeyDown(Engine::IEvent* pEvent);

    bool m_failure = false;
    std::string m_filenameToLoad;
    glm::vec2 m_camera = {0,0};
    glm::vec2 m_cameraTarget = {0,0};
    float m_zoom = 128.0f;
    float m_zoomTarget = 128.0f;
};
