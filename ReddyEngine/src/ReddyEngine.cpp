#include "Engine/ReddyEngine.h"
#include "Engine/Config.h"
#include "Engine/Log.h"

#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl3.h>
#include <SDL.h>
#include <SDL_opengl.h>

#include <cassert>


namespace Engine
{
    void Run(const std::shared_ptr<IGame>& pGame, int argc, const char** argv)
    {
        // Don't use CORE_ERROR etc. before spdlog initialization 
        Log::Init();

        // Load configs
        Config::load();

        // Initialize SDL
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO) != 0)
        {
            CORE_FATAL("Error: {} \n", SDL_GetError());
            assert(false);
            return;
        }

        // Save configs (It will only save if changes have been made)
        Config::save();
    }
}
