#include "Game.h"

#include <Engine/ReddyEngine.h>


int main(int argc, const char** argv)
{
    g_pGame = std::make_shared<Game>();
    Engine::Run(g_pGame, argc, argv);
    return 0;	
}
