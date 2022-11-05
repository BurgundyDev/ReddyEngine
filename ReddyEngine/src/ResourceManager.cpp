#include "Engine/Utils.h"
#include "Engine/Sound.h"
#include "Engine/Music.h"
#include "Engine/ResourceManager.h"

#include "Engine/Log.h"
#include "Engine/Texture.h"

#define RM_USE_FILENAME_ONLY

namespace Engine
{
		SoundRef ResourceManager::getSound(const std::string& name)
		{
#ifdef RM_USE_FILENAME_ONLY
			std::string& filename = Utils::findFile(name, "assets", true, true);

			if(filename.empty())
			{
				CORE_ERROR("File " + name + " not found");
				static SoundRef empty;
				return empty;
			}

			const SoundRef& sound = Sound::createFromFile(filename);
#else
			const SoundRef& sound = Sound::createFromFile(name);
#endif

			return sound;
		}

		MusicRef ResourceManager::getMusic(const std::string& name)
		{
#ifdef RM_USE_FILENAME_ONLY
			std::string& filename = Utils::findFile(name, "assets", true, true);

			if (filename.empty())
			{
				CORE_ERROR("File" + name + "not found");
				static MusicRef empty;
				return empty;
			}

			const MusicRef& music = Music::createFromFile(filename);
#else
			const MusicRef& music = Music::createFromFile(name);
#endif
			return music;
		}

		TextureRef ResourceManager::getTexture(const std::string& name)
		{
#ifdef RM_USE_FILENAME_ONLY
			std::string& filename = Utils::findFile(name, "assets", true, true);

			if (filename.empty())
			{
				CORE_ERROR("File" + name + "not found");
				static TextureRef empty;
				return empty;
			}

			const TextureRef& texture = Texture::createFromFile(filename);
#else
			const TextureRef& texture = Texture::createFromFile(name);

#endif
			return texture;
		}
}
