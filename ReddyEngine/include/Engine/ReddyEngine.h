#pragma once

#include "Engine/IGame.h"

#include <memory>

namespace Engine
{
    void Run(const std::shared_ptr<IGame>& pGame, int argc, const char** argv);
}
