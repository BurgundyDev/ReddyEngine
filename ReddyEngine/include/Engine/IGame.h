#pragma once

namespace Engine
{
    class IGame
    {
    public:
        virtual ~IGame() {}

        virtual void loadContent() = 0;
        virtual void update(float deltatime) = 0;
        virtual void fixedUpdate(float deltatime) = 0;
        virtual void draw() = 0;
    };
}
