#pragma once

#include "GameState.h"

#include <json/json.h>

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
    void hideInGameMenu();

private:
    enum class SubState
    {
        InGame,
        InGameMenu
    };

    void onKeyDown(Engine::IEvent* pEvent);
    void showInGameMenu();

    SubState m_subState = SubState::InGame;
    Json::Value m_inGameMenuJson;
    Engine::EntityRef m_pInGameMenu;
};
