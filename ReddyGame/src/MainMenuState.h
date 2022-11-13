#pragma once

#include "GameState.h"


class MainMenuState final : public GameState
{
public:
    MainMenuState();
    ~MainMenuState();

    void enter(const GameStateRef& previousState) override;
};
