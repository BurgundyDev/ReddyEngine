#pragma once

#include "Engine/IGame.h"

#include <glm/vec2.hpp>

#include <memory>


namespace Engine
{
    // Forward declarations
    class SpriteBatch;
    using SpriteBatchRef = std::shared_ptr<SpriteBatch>;
    class Input;
    using InputRef = std::shared_ptr<Input>;
    class Audio;
    using AudioRef = std::shared_ptr<Audio>;
    class ResourceManager;
    using ResourceManagerRef = std::shared_ptr<ResourceManager>;


    const SpriteBatchRef& getSpriteBatch();
    const InputRef& getInput();
    const AudioRef& getAudio();
    const ResourceManagerRef& getResourceManager();


    void Run(const std::shared_ptr<IGame>& pGame, int argc, const char** argv);
    glm::vec2 getResolution();
    void setFixedUpdateFPS(int fps = 60);
    void quit();
}
