#pragma once

#include <Engine/IGame.h>

class Game : public Engine::IGame
{
public:
    enum State
    {
        MainMenu,
        InGame,
        EditMode
    };

    Game() {}
    ~Game() {}

    void loadContent() override;
    void update(float deltatime) override;
    void fixedUpdate(float deltatime) override;
    void draw() override;

    State getState() const { return m_state; }

private:
    State m_state = State::MainMenu;
};
