#pragma once

#include <memory>


class GameState;
using GameStateRef = std::shared_ptr<GameState>;


class GameState
{
public:
    virtual ~GameState() {}

    bool isSeeThrough() const { return m_seeThrough; }

    virtual void update(float dt) {}
    virtual void fixedUpdate(float dt) {}
    virtual void draw() {}

    virtual void enter(const GameStateRef& previousState) {}
    virtual void leave(const GameStateRef& newState) {}

protected:
    bool m_seeThrough = false;
};
