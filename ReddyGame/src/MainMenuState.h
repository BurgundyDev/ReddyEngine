#pragma once

#include "GameState.h"

#include <glm/vec2.hpp>
#include <string>
#include <SDL_events.h>


class MainMenuState final : public GameState
{
public:
    MainMenuState();
    ~MainMenuState();

    void update(float dt) override;
    void draw() override;

    void enter(const GameStateRef& previousState) override;

private:
    bool m_saveExists = false;
};
