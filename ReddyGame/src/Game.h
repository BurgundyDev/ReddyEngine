#pragma once

#include <Engine/IGame.h>

class Game : public Engine::IGame
{
public:
    Game() {}
    ~Game() {}

    void Update(float deltatime) override {}
    void FixedUpdate(float deltatime) override {}
    void Draw() override {}
};
