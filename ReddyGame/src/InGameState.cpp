#include "InGameState.h"
#include "InGameMenuState.h"
#include "Game.h"

#include <Engine/Input.h>
#include <Engine/ReddyEngine.h>


void InGameState::update(float dt)
{
    if (Engine::getInput()->isKeyJustDown(SDL_SCANCODE_ESCAPE))
        g_pGame->pushState(std::make_shared<InGameMenuState>());
}
