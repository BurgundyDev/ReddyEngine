#pragma once
#include "Engine/Utils.h"
#include "Engine/Sound.h"
#include "Engine/Music.h"
#include "Engine/Texture.h"
#include <unordered_map>

namespace Engine
{
	class ResourceManager
	{
	public:
		SoundRef getSound(const std::string& filename);
		MusicRef getMusic(const std::string& name);
		TextureRef getTexture(const std::string& name);
	private:
		std::unordered_map<SoundRef, Sound> loadedSounds;
	};
}
