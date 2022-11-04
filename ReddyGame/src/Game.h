#pragma once

#include <Engine/IGame.h>

class Game : public Engine::IGame
{
public:
    Game() {}
    ~Game() {}

    void loadContent() override;
    void update(float deltatime) override;
    void fixedUpdate(float deltatime) override;
    void draw() override;
};
