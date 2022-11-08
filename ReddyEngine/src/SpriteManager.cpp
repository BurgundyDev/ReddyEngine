#include "Engine/SpriteManager.h"


namespace Engine
{
    void SpriteManager::clear()
    {
        m_sprites.clear();
    }

    void SpriteManager::registerSprite(const SpriteComponentRef& pSprite)
    {
        m_sprites.push_back(pSprite);
    }

    void SpriteManager::deregisterSprite(const SpriteComponentRef& pSprite)
    {
        for (auto it = m_sprites.begin(); it != m_sprites.end(); ++it)
        {
            if (*it == pSprite)
            {
                m_sprites.erase(it);
                return;
            }
        }
    }

    void SpriteManager::draw()
    {
    }
}
