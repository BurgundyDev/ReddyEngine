#include "Engine/Utils.h"
#include "Engine/Sound.h"
#include "Engine/Music.h"
#include "Engine/ResourceManager.h"

#include "Engine/Log.h"
#include "Engine/Texture.h"


#include <string>
#include <unordered_map>

#define RM_USE_FILENAME_ONLY

namespace Engine
{
		SoundRef ResourceManager::getSound(const std::string& name)
		{
			SoundRef sound;
#ifdef RM_USE_FILENAME_ONLY
			std::string& filename = Utils::findFile(name, "assets", true, true);

			if(filename.empty())
			{
				CORE_ERROR("File " + name + " not found");
				static SoundRef empty;
				return empty;
			}

			if(loadedSounds.find(filename) == loadedSounds.end())
			{
				sound = Sound::createFromFile(filename);
				loadedSounds[filename] = sound;
			}
			else
			{
				sound = loadedSounds[filename];
			}
#else
			if (loadedSounds.find(name) == loadedSounds.end())
			{
				sound = Sound::createFromFile(name);
				loadedSounds[name] = sound;
			}
			else
			{
				sound = loadedSounds[name];
			}
#endif

			return sound;
		}

		MusicRef ResourceManager::getMusic(const std::string& name)
		{
			MusicRef music;
#ifdef RM_USE_FILENAME_ONLY
			std::string& filename = Utils::findFile(name, "assets", true, true);

			if (filename.empty())
			{
				CORE_ERROR("File" + name + "not found");
				static MusicRef empty;
				return empty;
			}

			if (loadedMusic.find(filename) == loadedMusic.end())
			{
				music = Music::createFromFile(filename);
				loadedMusic[filename] = music;
			}
			else
			{
				music = loadedMusic[filename];
			}
#else
			if (loadedMusic.find(name) == loadedMusic.end())
			{
				music = Music::createFromFile(name);
				loadedMusic[name] = music;
			}
			else
			{
				music = loadedMusic[name];
			}
#endif
			return music;
		}

		TextureRef ResourceManager::getTexture(const std::string& name)
		{
			TextureRef texture;
#ifdef RM_USE_FILENAME_ONLY
			std::string& filename = Utils::findFile(name, "assets", true, true);

			if (filename.empty())
			{
				CORE_ERROR("File" + name + "not found");
				static TextureRef empty;
				return empty;
			}

			if (loadedTextures.find(filename) == loadedTextures.end())
			{
				texture = Texture::createFromFile(filename);
				loadedTextures[filename] = texture;
			}
			else
			{
				texture = loadedTextures[filename];
			}
#else
			if (loadedTextures.find(name) == loadedTextures.end())
			{
				texture = Texture::createFromFile(name);
				loadedTextures[name] = texture;
			}
			else
			{
				texture = loadedTextures[name];
			}
#endif
			return texture;
		}
}
