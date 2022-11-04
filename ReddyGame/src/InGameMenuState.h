#pragma once

#include "GameState.h"


class InGameMenuState final : public GameState
{
public:
    InGameMenuState();

    void update(float dt) override;
};
