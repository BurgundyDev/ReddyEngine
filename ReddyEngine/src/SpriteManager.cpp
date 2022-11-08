#include "Engine/SpriteManager.h"
#include "Engine/ReddyEngine.h"
#include "Engine/SpriteBatch.h"
#include "Engine/SpriteComponent.h"
#include "Engine/Entity.h"
#include "Engine/Constants.h"


namespace Engine
{
    void SpriteManager::clear()
    {
        m_sprites.clear();
    }

    void SpriteManager::registerSprite(SpriteComponent* pSprite)
    {
        m_sprites.push_back(pSprite);
    }

    void SpriteManager::deregisterSprite(SpriteComponent* pSprite)
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
        //TODO: Layers and chunks n shit

        auto sb = getSpriteBatch().get();

        for (auto pSprite : m_sprites)
        {
            const auto& transform = pSprite->getEntity()->getTransform();

            sb->draw(pSprite->pTexture,
                     transform.position,
                     pSprite->color, 
                     transform.rotation,
                     transform.scale * SPRITE_BASE_SCALE,
                     pSprite->origin,
                     pSprite->uvs);
        }
    }
}
