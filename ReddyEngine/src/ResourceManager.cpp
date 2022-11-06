#include "Engine/Utils.h"
#include "Engine/Sound.h"
#include "Engine/Music.h"
#include "Engine/Texture.h"
#include "Engine/PFX.h"
#include "Engine/Font.h"

#include "Engine/ResourceManager.h"

#include "Engine/Log.h"
#include "Engine/Resource.h"


#include <string>
#include <unordered_map>

namespace Engine
{
    SoundRef ResourceManager::getSound(const std::string& name)
    {
        return getResource<Sound>(name);
    }

    MusicRef ResourceManager::getMusic(const std::string& name)
    {
        return getResource<Music>(name);
    }

    TextureRef ResourceManager::getTexture(const std::string& name)
    {
        return getResource<Texture>(name);
    }

    PFXRef ResourceManager::getPFX(const std::string& name)
    {
        return getResource<PFX>(name);
    }

    FontRef ResourceManager::getFont(const std::string& name, int height)
    {
        return getResource<Font>(name, height);
    }
}
