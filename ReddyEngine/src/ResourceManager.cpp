#include "Engine/Utils.h"
#include "Engine/Sound.h"
#include "Engine/Music.h"
#include "Engine/ResourceManager.h"

#include "Engine/Texture.h"

namespace Engine
{
		SoundRef ResourceManager::getSound(const std::string& name)
		{
			const std::string& filename = Utils::findFile(name, "assets", true, true);

			const SoundRef& sound = Sound::createFromFile(filename);

			return sound;
		}

		MusicRef ResourceManager::getMusic(const std::string& name)
		{
			const std::string& filename = Utils::findFile(name, "assets", true, true);

			const MusicRef& music = Music::createFromFile(filename);

			return music;
		}

		TextureRef ResourceManager::getTexture(const std::string& name)
		{
			const std::string& filename = Utils::findFile(name, "assets", true, true);

			const TextureRef& texture = Texture::createFromFile(filename);

			return texture;
		}
}
