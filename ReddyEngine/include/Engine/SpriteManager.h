#pragma once

#include <memory>
#include <vector>


namespace Engine
{
    class SpriteComponent;


    class SpriteManager final
    {
    public:
        void clear();

        void registerSprite(SpriteComponent* pSprite);
        void deregisterSprite(SpriteComponent* pSprite);

        void draw();

    private:
        std::vector<SpriteComponent*> m_sprites;
    };
}
