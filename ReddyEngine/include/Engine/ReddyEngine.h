#pragma once

#include "Engine/IGame.h"

#include <glm/vec2.hpp>

#include <memory>


namespace Engine
{
    // Forward declaration
    class SpriteBatch;
    using SpriteBatchRef = std::shared_ptr<SpriteBatch>;


    void Run(const std::shared_ptr<IGame>& pGame, int argc, const char** argv);

    const SpriteBatchRef& getSpriteBatch();
    glm::ivec2 getResolution();
}
