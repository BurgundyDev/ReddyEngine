#include "Engine/ReddyEngine.h"

//#include <backends/imgui_impl_sdl.h>
//#include <backends/imgui_impl_opengl3.h>
#include <SDL.h>
//#include <SDL_opengl.h>

#include <cassert>


namespace Engine
{
    void Run(const std::shared_ptr<IGame>& pGame, int argc, const char** argv)
    {
        // Main loop, initialize SDL, bla bla bla
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO) != 0)
        {
            printf("Error: %s\n", SDL_GetError());
            assert(false);
            return;
        }

		/*
        while (true)
			pGame->Update(...);
		    pGame->Draw()
        */
    }
}
