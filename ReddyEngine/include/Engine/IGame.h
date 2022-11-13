#pragma once

#include <string>


namespace Engine
{
    enum StateChangeRequest
    {
        None,
        ContinueGame,
        NewGame,
        Quit,
        Resume,
        Editor,
        MainMenu
    };

    class IGame
    {
    public:
        virtual ~IGame() {}

        virtual void loadContent() = 0;
        virtual void update(float deltatime) = 0;
        virtual void fixedUpdate(float deltatime) = 0;
        virtual void draw() = 0;
        virtual void changeState(StateChangeRequest stateChangeRequest, const std::string& filename) = 0; // Called from Lua
    };
}
