#pragma once

#include <glm/vec2.hpp>

#include <memory>
#include <string>


class GameState;
using GameStateRef = std::shared_ptr<GameState>;


class GameState
{
public:
    GameState(const std::string& filename);
    virtual ~GameState() {}

    bool isSeeThrough() const { return m_seeThrough; }

    virtual void update(float dt);
    virtual void fixedUpdate(float dt);
    virtual void draw();

    virtual void enter(const GameStateRef& previousState);
    virtual void leave(const GameStateRef& newState);

    glm::vec2 camera = glm::vec2(0);
    float zoom = 128.0f;

protected:
    bool m_seeThrough = false;
    std::string m_filenameToLoad;
};
