#pragma once
#include "Engine/Utils.h"
#include "Engine/Sound.h"
#include "Engine/Music.h"
#include "Engine/Texture.h"
#include "Engine/Font.h"
#include "Resource.h"
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
