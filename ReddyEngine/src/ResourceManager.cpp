#include "Engine/Utils.h"
#include "Engine/Sound.h"
#include "Engine/Music.h"
#include "Engine/Texture.h"

namespace Engine
{
	class ResourceManager
	{
	public:
		SoundRef getSound(const std::string& name)
		{
			const std::string& filename = Utils::findFile(name, "assets", true, true);

			const SoundRef& sound = Sound::createFromFile(filename);

			return sound;
		}

		MusicRef getMusic(const std::string& name)
		{
			const std::string& filename = Utils::findFile(name, "assets", true, true);

			const MusicRef& music = Music::createFromFile(filename);

			return music;
		}

		TextureRef getTexture(const std::string& name)
		{
			const std::string& filename = Utils::findFile(name, "assets", true, true);

			const TextureRef& texture = Texture::createFromFile(filename);

			return texture;
		}
	};
}
