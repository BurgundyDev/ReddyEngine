#pragma once

#include <Engine/IGame.h>
#include "Engine/ResourceManager.h"

#include <memory>
#include <vector>

class GameState;
using GameStateRef = std::shared_ptr<GameState>;


class Game : public Engine::IGame
{
public:
    Game() {}
    ~Game() {}

    void loadContent() override;
    void update(float deltatime) override;
    void fixedUpdate(float deltatime) override;
    void draw() override;
    void changeState(Engine::StateChangeRequest stateChangeRequest, const std::string& filename) override;

    void changeState(const GameStateRef& newState);
    void pushState(const GameStateRef& newState);
    void popState();

private:
    std::vector<GameStateRef> m_pGameStates;
};
