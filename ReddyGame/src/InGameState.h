#pragma once

#include "GameState.h"

#include <memory>


namespace Engine
{
    class IEvent;

    class Entity;
    using EntityRef = std::shared_ptr<Entity>;
};


class InGameState final : public GameState
{
public:
    InGameState(const std::string& filename);

    void enter(const GameStateRef& previousState) override;
    void leave(const GameStateRef& newState) override;

private:
    void onKeyDown(Engine::IEvent* pEvent);
};
