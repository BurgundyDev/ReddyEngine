#include "Engine/Utils.h"
#include "Engine/Sound.h"
#include "Engine/Music.h"
#include "Engine/ResourceManager.h"

#include "Engine/Log.h"
#include "Engine/Texture.h"

namespace Engine
{
		SoundRef ResourceManager::getSound(const std::string& name)
		{
			const std::string& filename = Utils::findFile(name, "assets", true, true);

			if(filename.empty())
			{
				CORE_ERROR("File " + name + " not found");
				static SoundRef empty;
				return empty;
			}

			const SoundRef& sound = Sound::createFromFile(filename);

			return sound;
		}

		MusicRef ResourceManager::getMusic(const std::string& name)
		{
			const std::string& filename = Utils::findFile(name, "assets", true, true);

			if (filename.empty())
			{
				CORE_ERROR("File" + name + "not found");
				static MusicRef empty;
				return empty;
			}

			const MusicRef& music = Music::createFromFile(filename);

			return music;
		}

		TextureRef ResourceManager::getTexture(const std::string& name)
		{
			const std::string& filename = Utils::findFile(name, "assets", true, true);

			if (filename.empty())
			{
				CORE_ERROR("File" + name + "not found");
				static TextureRef empty;
				return empty;
			}

			const TextureRef& texture = Texture::createFromFile(filename);

			return texture;
		}
}
