#include "Game.h"

#include <Engine/ReddyEngine.h>


int main(int argc, const char** argv)
{
    auto pGame = std::make_shared<Game>();
    Engine::Run(pGame, argc, argv);
    return 0;	
}
