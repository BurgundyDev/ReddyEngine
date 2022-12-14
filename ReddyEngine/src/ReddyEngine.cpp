#include "Engine/Audio.h"
#include "Engine/ReddyEngine.h"
#include "Engine/Config.h"
#include "Engine/Input.h"
#include "Engine/Log.h"
#include "Engine/SpriteBatch.h"
#include "Engine/ResourceManager.h"
#include "Engine/Scene.h"
#include "Engine/EventSystem.h"
#include "Engine/LuaBindings.h"
#include "Engine/MusicManager.h"
#include "Engine/ComponentFactory.h"
#include "Engine/Entity.h"
#include "Engine/Scene.h"
#include "Engine/Font.h"

#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl3.h>
#include <SDL.h>
#include <SDL_opengl.h>

#include <cassert>


namespace Engine
{
    static SpriteBatchRef g_pSpriteBatch;
    static InputRef g_pInput;
    static AudioRef g_pAudio;
    static ResourceManagerRef g_pResourceManager;
    static SceneRef g_pScene;
	static EventSystemRef g_pEventSystem;
	static LuaBindingsRef g_pLuaBindings;
	static IGameRef g_pGame;
	static MusicManagerRef g_pMusicManager;
	static FontRef g_pFPSFont;

    static int g_fixedUpdateFPS = 60;
    static bool g_done = false;
    static SDL_Window* pWindow = nullptr;


    void Run(const std::shared_ptr<IGame>& pGame, int argc, const char** argv)
    {
        g_pGame = pGame;

        // Don't use CORE_ERROR etc. before spdlog initialization 
        Log::Init();
        
        // Load configs
        Config::load();

        // Initialize SDL
        {
            auto success = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO) == 0;
            SDL_GLContext gl_context = SDL_GL_CreateContext(NULL);
            CORE_ASSERT(success, "Error: {}", SDL_GetError());
        }

      
#if defined(__APPLE__)
        // GL 3.2 Core + GLSL 150
        const char* glsl_version = "#version 150";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
        // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif
#if defined(WIN32)
        if (Config::dpiAware)
            SetProcessDPIAware();
#endif

        // Create window with graphics context
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_WindowFlags window_flags = (SDL_WindowFlags)(
            SDL_WINDOW_OPENGL | 
            SDL_WINDOW_ALLOW_HIGHDPI | /* This flag doesn't do anything on Windows, SDL doesnt implement it */
            SDL_WINDOW_RESIZABLE
        );
        pWindow = SDL_CreateWindow("Reddy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Config::resolution.x, Config::resolution.y, window_flags);
        displayModeChanged();
        
        // enable file drop events
        SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

        SDL_GLContext gl_context = SDL_GL_CreateContext(pWindow);
        SDL_GL_MakeCurrent(pWindow, gl_context);
        SDL_GL_SetSwapInterval(Config::vsync ? 1 : 0);

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        
        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        // Setup Platform/Renderer backends
        ImGui_ImplSDL2_InitForOpenGL(pWindow, gl_context);
        ImGui_ImplOpenGL3_Init(glsl_version);

        // Load Fonts
        // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
        // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
        // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
        // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
        // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
        // - Read 'docs/FONTS.md' for more instructions and details.
        // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
        //io.Fonts->AddFontDefault();
        //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
        //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
        //IM_ASSERT(font != NULL);

        if (Utils::fileExists("c:\\Windows\\Fonts\\segoeui.ttf"))
            io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f, NULL);

        // Initialize Engine's systems
        ComponentFactory::initialize();
        g_pEventSystem = std::make_shared<EventSystem>();
        g_pInput = std::make_shared<Input>();
        g_pAudio = std::make_shared<Audio>();
        g_pSpriteBatch = std::make_shared<SpriteBatch>();
        g_pResourceManager = std::make_shared<ResourceManager>();
        g_pMusicManager = std::make_shared<MusicManager>();
        g_pScene = std::make_shared<Scene>();
        g_pLuaBindings = std::make_shared<LuaBindings>();
        g_pLuaBindings->init();
        g_pScene->init();

        g_pFPSFont = g_pResourceManager->getFont("fonts/defaultFont24.json");

        // Once everything is setup, the game can load stuff
        pGame->loadContent();

        // Main loop
        Uint64 lastTime = SDL_GetPerformanceCounter();
        float fixedUpdateProgress = 0.0f;
        int currentFPS = 0;
        int fps = 0;
        float fpsDelay = 0.0f;
        while (!g_done)
        {
            g_pEventSystem->dispatchEvents();
            
            Sint32 mouseMotionX = 0;
            Sint32 mouseMotionY = 0;

            g_pInput->preUpdate();

            // Poll and handle events (inputs, pWindow resize, etc.)
            // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
            // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
            // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
            // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
				g_pEventSystem->sendSDLEvent(&event);

                ImGui_ImplSDL2_ProcessEvent(&event);

                switch (event.type)
                {
                    case SDL_QUIT:
                        g_done = true;
                        break;

                    case SDL_WINDOWEVENT:
                    {
                        if (event.window.windowID == SDL_GetWindowID(pWindow))
                        {
                            switch (event.window.event)
                            { // Holy indentations!
                                case SDL_WINDOWEVENT_CLOSE:
                                    g_done = true;
                                    break;
                                case SDL_WINDOWEVENT_RESIZED:
                                {
                                    if (Config::displayMode == Config::DisplayMode::Windowed)
                                        SDL_GetWindowSize(pWindow, &Config::resolution.x, &Config::resolution.y);
                                    break;
                                }
                            }
                        }
                        break;
                    }

                case SDL_KEYDOWN:
                    if (!event.key.repeat && !io.WantCaptureKeyboard)
                        g_pInput->onKeyDown(event.key.keysym.scancode);
                    break;

                case SDL_KEYUP:
                    if (!io.WantCaptureKeyboard)
                        g_pInput->onKeyUp(event.key.keysym.scancode);
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if (!io.WantCaptureMouse)
                        g_pInput->onButtonDown(event.button.button);
                    break;

                case SDL_MOUSEBUTTONUP:
                    if (!io.WantCaptureMouse)
                        g_pInput->onButtonUp(event.button.button);
                    break;

                case SDL_MOUSEWHEEL:
                    g_pInput->setMouseWheelMotion(event.wheel.y);
                    break;

                case SDL_MOUSEMOTION:
                    mouseMotionX += event.motion.xrel;
                    mouseMotionY += event.motion.yrel;
                    g_pInput->onMouseMove({event.motion.x, event.motion.y});
                    break;
                }
            }
            if (g_done) break;

            g_pEventSystem->dispatchEvents();

            // Update inputs
            g_pInput->setMouseMotion({mouseMotionX, mouseMotionY});
            g_pInput->update();

            g_pEventSystem->dispatchEvents();

            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();

            // Calculate elapsed time since last frame
            auto now = SDL_GetPerformanceCounter();
            auto deltaTime = (float)((now - lastTime) / (double)SDL_GetPerformanceFrequency());
            if (deltaTime > 1.0f / 10.0f) deltaTime = 1.0f / 10.0f;
            lastTime = now;

            g_pEventSystem->dispatchEvents();

            // Fixed update shenanigans
            int fixedUpdated = 0;
            fixedUpdateProgress += deltaTime;
            while (fixedUpdateProgress > 0.0f)
            {
                float fixedUpdateTime = 1.0f / (float)g_fixedUpdateFPS;
                
                //g_pEventSystem->dispatchEvents();
                //g_pScene->fixedUpdate(fixedUpdateTime);
                
                g_pEventSystem->dispatchEvents();
                pGame->fixedUpdate(fixedUpdateTime);
                
                g_pEventSystem->dispatchEvents();
                g_pLuaBindings->fixedUpdate(fixedUpdateTime);
                
                fixedUpdateProgress -= 1.0f / (float)g_fixedUpdateFPS;
                ++fixedUpdated;
                if (fixedUpdated > 3)
                {
                    // Things got too slow, start slowing down.
                    fixedUpdateProgress = 0.0f;
                    break;
                }
            }

            // Update
            //g_pEventSystem->dispatchEvents();
            //g_pScene->update(deltaTime);

            g_pEventSystem->dispatchEvents();
            pGame->update(deltaTime);

            g_pEventSystem->dispatchEvents();
            g_pLuaBindings->update(deltaTime);

            // Generate imgui final render data
            ImGui::Render();

            // Prepare rendering
            glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT);
            g_pSpriteBatch->beginFrame();

            // Draw game
            g_pEventSystem->dispatchEvents();
            pGame->draw();

            // FPS
            ++currentFPS;
            fpsDelay += deltaTime;
            if (fpsDelay >= 1.0f)
            {
                fpsDelay -= 1.0f;
                fps = currentFPS;
                currentFPS = 0;
            }
            if (Config::showFPS)
            {
                g_pSpriteBatch->begin();
                g_pFPSFont->draw("FPS: " + std::to_string(fps), {0, 25});
                g_pSpriteBatch->end();
            }
            
            // Draw ImGui on top
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // Swap (Present)
            SDL_GL_SwapWindow(pWindow);
        }

        // Save configs (It will only save if changes have been made)
        Config::save();

        // Cleanup
        g_pFPSFont.reset();
        g_pLuaBindings.reset();
        g_pScene.reset();
        g_pMusicManager.reset();
        g_pResourceManager.reset();
        g_pSpriteBatch.reset();
        g_pAudio.reset();
        g_pInput.reset();
        g_pEventSystem.reset();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_GL_DeleteContext(gl_context);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();

        g_pGame = nullptr;
    }

    void setWindowCaption(const std::string& caption)
    {
        SDL_SetWindowTitle(pWindow, caption.c_str());
    }

    const IGameRef& getGame()
    {
        return g_pGame;
    }

    const SpriteBatchRef& getSpriteBatch()
    {
        return g_pSpriteBatch;
    }

    const InputRef& getInput()
    {
        return g_pInput;
    }

    const AudioRef& getAudio()
    {
        return g_pAudio;
    }

    const ResourceManagerRef& getResourceManager()
    {
        return g_pResourceManager;
    }

	const SceneRef& getScene()
	{
        return g_pScene;
	}

	const Engine::EventSystemRef& getEventSystem()
	{
        return g_pEventSystem;
	}

	const Engine::LuaBindingsRef& getLuaBindings()
	{
        return g_pLuaBindings;
	}

    const MusicManagerRef& getMusicManager()
    {
        return g_pMusicManager;
    }

	glm::vec2 getResolution()
    {
        const auto& io = ImGui::GetIO();
        return { io.DisplaySize.x, io.DisplaySize.y };
    }

    void setFixedUpdateFPS(int fps)
    {
        g_fixedUpdateFPS = fps;
    }

    void quit()
    {
        g_done = true;
    }

    void displayModeChanged()
    {
        switch (Config::displayMode)
        {
            case Config::DisplayMode::Windowed:
            {
                SDL_SetWindowFullscreen(pWindow, 0);
                break;
            }
            case Config::DisplayMode::BorderlessFullscreen:
            {
                SDL_SetWindowFullscreen(pWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
                break;
            }
        }
    }

    void vsyncChanged()
    {
        SDL_GL_SetSwapInterval(Config::vsync ? 1 : 0);
    }
}
