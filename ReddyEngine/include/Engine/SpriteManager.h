#pragma once

#include <memory>
#include <vector>


namespace Engine
{
    class SpriteComponent;
    using SpriteComponentRef = std::shared_ptr<SpriteComponent>;


    class SpriteManager final
    {
    public:
        void clear();

        void registerSprite(const SpriteComponentRef& pSprite);
        void deregisterSprite(const SpriteComponentRef& pSprite);

        void draw();

    private:
        std::vector<SpriteComponentRef> m_sprites;
    };
}
