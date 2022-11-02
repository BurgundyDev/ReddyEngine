#pragma once

namespace Engine
{
    class IGame
    {
    public:
        virtual ~IGame() {}

        virtual void Update(float deltatime) = 0;
        virtual void FixedUpdate(float deltatime) = 0;
        virtual void Draw() = 0;
    };
}
