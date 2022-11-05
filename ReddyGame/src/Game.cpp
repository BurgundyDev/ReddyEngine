#include "Game.h"
#include "MainMenuState.h"

#include <Engine/ReddyEngine.h>
#include <Engine/SpriteBatch.h>
#include <Engine/Texture.h>

#include <imgui.h>

#include "Engine/ResourceManager.h"


GameRef g_pGame;


void Game::loadContent()
{
    changeState(std::make_shared<MainMenuState>());

    pResourceManager = std::make_shared<Engine::ResourceManager>();
}

void Game::changeState(const GameStateRef& newState)
{
    GameStateRef prevState = nullptr;
    while (!m_pGameStates.empty())
    {
        prevState = m_pGameStates.back();
        m_pGameStates.pop_back();
        prevState->leave(newState);
    }

    m_pGameStates.push_back(newState);
    newState->enter(prevState);
}


void Game::pushState(const GameStateRef& newState)
{
    GameStateRef prevState = nullptr;
    if (!m_pGameStates.empty())
    {
        prevState = m_pGameStates.back();
        prevState->leave(newState);
    }

    m_pGameStates.push_back(newState);
    newState->enter(prevState);
}


void Game::popState()
{
    GameStateRef newState = nullptr;
    GameStateRef prevState = nullptr;

    if (m_pGameStates.size() > 2) newState = m_pGameStates[m_pGameStates.size() - 2];
    if (m_pGameStates.size() > 1) prevState = m_pGameStates[m_pGameStates.size() - 1];

    m_pGameStates.pop_back();

    if (prevState) prevState->leave(newState);
    if (newState) newState->enter(prevState);
}


void Game::update(float deltaTime)
{
    if (!m_pGameStates.empty()) m_pGameStates.back()->update(deltaTime);
}


void Game::fixedUpdate(float deltaTime)
{
    if (!m_pGameStates.empty()) m_pGameStates.back()->fixedUpdate(deltaTime);
}


void Game::draw()
{
    if (m_pGameStates.empty()) return;

    // Find index where all parents are see-through
    int index = (int)m_pGameStates.size() - 1;
    for (auto rit = m_pGameStates.rbegin(); rit != m_pGameStates.rend(); ++rit)
    {
        const auto& pGameState = *rit;
        if (!pGameState->isSeeThrough()) break;
        --index;
    }

    for (int i = index, len = (int)m_pGameStates.size(); i < len; ++i)
    {
        const auto& pGameState = m_pGameStates[i];
        pGameState->draw();
    }
}
