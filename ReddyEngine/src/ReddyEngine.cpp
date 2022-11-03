#include "Engine/ReddyEngine.h"
#include "Engine/Log.h"

//#include <backends/imgui_impl_sdl.h>
//#include <backends/imgui_impl_opengl3.h>
#include <SDL.h>
//#include <SDL_opengl.h>

#include <cassert>


namespace Engine
{
    void Run(const std::shared_ptr<IGame>& pGame, int argc, const char** argv)
    {
        // Main loop, initialize spdlog, SDL, bla bla bla
        // Don't use CORE_ERROR etc. before spdlog initialization 
        Log::Init();

        CORE_INFO("Initializing Engine.");
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO) != 0)
        {
            CORE_ERROR("Error: {} \n", SDL_GetError());
            assert(false);
            return;
        }

        CORE_INFO("Starting the game loop.");
		/*
        while (true)
			pGame->Update(...);
		    pGame->Draw()
        */
        CORE_INFO("Closing Engine.");
    }
}
