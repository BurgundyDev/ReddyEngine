#pragma once

#include "GameState.h"

#include <string>


namespace Engine
{
    class IEvent;
};


class InGameState final : public GameState
{
public:
    InGameState(const std::string& filename);

    void enter(const GameStateRef& previousState) override;
    void leave(const GameStateRef& newState) override;

private:
    void onKeyDown(Engine::IEvent* pEvent);

    bool m_failure = false;
    std::string m_filenameToLoad;
};
