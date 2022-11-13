#pragma once

#include "Engine/IGame.h"

#include <glm/vec2.hpp>

#include <memory>
#include <string>


namespace Engine
{
    static const int FILES_VERSION = 1;


    // Forward declarations
    class SpriteBatch;
    using SpriteBatchRef = std::shared_ptr<SpriteBatch>;

    class Input;
    using InputRef = std::shared_ptr<Input>;

    class Audio;
    using AudioRef = std::shared_ptr<Audio>;

    class ResourceManager;
    using ResourceManagerRef = std::shared_ptr<ResourceManager>;

    class Scene;
    using SceneRef = std::shared_ptr<Scene>;

    class EventSystem;
    using EventSystemRef = std::shared_ptr<EventSystem>;

    class LuaBindings;
    using LuaBindingsRef = std::shared_ptr<LuaBindings>;

    class IGame;
    using IGameRef = std::shared_ptr<IGame>;

    class MusicManager;
    using MusicManagerRef = std::shared_ptr<MusicManager>;

    
    const IGameRef& getGame();
    const SpriteBatchRef& getSpriteBatch();
    const InputRef& getInput();
    const AudioRef& getAudio();
    const ResourceManagerRef& getResourceManager();
    const SceneRef& getScene();
	const EventSystemRef& getEventSystem();
	const LuaBindingsRef& getLuaBindings();
    const MusicManagerRef& getMusicManager();


    void Run(const std::shared_ptr<IGame>& pGame, int argc, const char** argv);
    glm::vec2 getResolution();
    void setFixedUpdateFPS(int fps = 60);
    void setWindowCaption(const std::string& caption);
    void quit();
}
