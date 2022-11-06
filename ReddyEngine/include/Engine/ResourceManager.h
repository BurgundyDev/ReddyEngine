#pragma once
#include "Engine/Utils.h"
#include "Engine/Sound.h"
#include "Engine/Music.h"
#include "Engine/Texture.h"
#include "Engine/Font.h"
#include "Engine/PFX.h"
#include "Engine/Resource.h"
#include <string>
#include <unordered_map>

namespace Engine
{
    class ResourceManager
    {
    public:
        SoundRef getSound(const std::string& name);
        MusicRef getMusic(const std::string& name);
        TextureRef getTexture(const std::string& name);
        PFXRef getPFX(const std::string& name);
        FontRef getFont(const std::string& name, int height);
    private:
        std::unordered_map<std::string, ResourceRef> loadedResources;

        template<typename Tresouce>
        std::shared_ptr<Tresouce> getResource(const std::string& name)
        {
            auto it = loadedResources.find(name);
            if (it == loadedResources.end())
            {
                std::shared_ptr<Tresouce> pRet = Tresouce::createFromFile(name);
                loadedResources[name] = pRet;
                return pRet;
            }
            else
            {
                return std::dynamic_pointer_cast<Tresouce>(it->second);
            }
        }

        // A template implementing height for fonts that use it for Font::createFromFile
        // A better and more versatile solution would be using a variable argument ResourceManager::getResource function, but as for now this only appears to be needed for fonts, no need to implement that.
        template<typename Tresouce>
        std::shared_ptr<Tresouce> getResource(const std::string& name, int height)
        {
            auto it = loadedResources.find(name);
            if (it == loadedResources.end())
            {
                std::shared_ptr<Tresouce> pRet = Tresouce::createFromFile(name, height);
                loadedResources[name] = pRet;
                return pRet;
            }
            else
            {
                return std::dynamic_pointer_cast<Tresouce>(it->second);
            }
        }
    };
}
