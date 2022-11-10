#pragma once

#include "GameState.h"


class InGameState final : public GameState
{
public:
    void update(float dt) override;

    void enter(const GameStateRef& previousState) override;
};
