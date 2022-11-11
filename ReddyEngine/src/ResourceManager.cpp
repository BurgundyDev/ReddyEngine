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
#include <filesystem>

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

    bool ResourceManager::copyFileToAssets(const std::string &path, const std::string& subDir, std::string& resultPath)
    {
        const std::filesystem::path selectedPath(path);
        const std::filesystem::path filename = selectedPath.filename();
        const std::filesystem::path assetsPath = std::filesystem::current_path() / "assets";
        const std::filesystem::path destinationPath = assetsPath / subDir / filename;

        if (selectedPath != destinationPath) {
            try {
                if (std::filesystem::copy_file(selectedPath, destinationPath)) {
                    const std::filesystem::path relativePath = std::filesystem::relative(destinationPath, assetsPath);

                    resultPath = relativePath.string();

                    return true;
                }
            } catch (...) {
                return false;
            }
        } else {
            const std::filesystem::path relativePath = std::filesystem::relative(destinationPath, assetsPath);

            resultPath = relativePath.string();

            return true;
        }

        return false;
    }
}
