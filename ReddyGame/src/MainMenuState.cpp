#include "MainMenuState.h"

#include <Engine/ReddyEngine.h>
#include <Engine/Input.h>


MainMenuState::MainMenuState()
    : GameState("assets/scenes/mainmenu.json")
{
}

MainMenuState::~MainMenuState()
{
}

void MainMenuState::enter(const GameStateRef& previousState)
{
    GameState::enter(previousState);
    Engine::getInput()->setDefaultCursor();
}
