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

#define RM_USE_FILENAME_ONLY

namespace Engine
{
    SoundRef ResourceManager::getSound(const std::string& name)
    {
#ifdef RM_USE_FILENAME_ONLY
        std::string filename = Utils::findFile(name, "assets", true, true);

        if(filename.empty())
        {
        CORE_ERROR("File " + name + " not found");
        static SoundRef empty;
        return empty;
        }

        return getResource<Sound>(filename);
#else
        return getResource<Sound>(name);
#endif
    }

    MusicRef ResourceManager::getMusic(const std::string& name)
    {
#ifdef RM_USE_FILENAME_ONLY
        std::string filename = Utils::findFile(name, "assets", true, true);

        if (filename.empty())
        {
        CORE_ERROR("File" + name + "not found");
        static MusicRef empty;
        return empty;
        }

        return getResource<Music>(filename);
#else
        return getResource<Music>(name);
#endif
    }

    TextureRef ResourceManager::getTexture(const std::string& name)
    {
#ifdef RM_USE_FILENAME_ONLY
        std::string filename = Utils::findFile(name, "assets", true, true);

        if (filename.empty())
        {
        CORE_ERROR("File" + name + "not found");
        static TextureRef empty;
        return empty;
        }

        return getResource<Texture>(filename);
#else
        return getResource<Texture>(name);
#endif
    }

    PFXRef ResourceManager::getPFX(const std::string& name)
    {
#ifdef RM_USE_FILENAME_ONLY
        std::string filename = Utils::findFile(name, "assets", true, true);

        if (filename.empty())
        {
            CORE_ERROR("File" + name + "not found");
            static PFXRef empty;
            return empty;
        }

        return getResource<PFX>(filename);
#else
        return getResource<PFX>(name);
#endif
    }


    FontRef ResourceManager::getFont(const std::string& name, int height)
    {
#ifdef RM_USE_FILENAME_ONLY
        std::string filename = Utils::findFile(name, "assets", true, true);

        if (filename.empty())
        {
            CORE_ERROR("File" + name + "not found");
            static FontRef empty;
            return empty;
        }

        return getResource<Font>(filename, height);
#else
        return getResource<Font>(name, height);
#endif
    }
}
