#include "Engine/Utils.h"
#include "Engine/Sound.h"
#include "Engine/Music.h"
#include "Engine/ResourceManager.h"

#include "Engine/Log.h"
#include "Engine/Texture.h"
#include "Engine/Resource.h"


#include <string>
#include <unordered_map>

#define RM_USE_FILENAME_ONLY

namespace Engine
{
	SoundRef ResourceManager::getSound(const std::string& name)
	{
		SoundRef sound;
#ifdef RM_USE_FILENAME_ONLY
		std::string filename = Utils::findFile(name, "assets", true, true);

		if(filename.empty())
		{
		CORE_ERROR("File " + name + " not found");
		static SoundRef empty;
		return empty;
		}

		sound = getResource<Sound>(sound, filename);
#else
		sound = getResource<Sound>(sound, name);
#endif

		return sound;
	}

	MusicRef ResourceManager::getMusic(const std::string& name)
	{
		MusicRef music;
#ifdef RM_USE_FILENAME_ONLY
		std::string filename = Utils::findFile(name, "assets", true, true);

		if (filename.empty())
		{
		CORE_ERROR("File" + name + "not found");
		static MusicRef empty;
		return empty;
		}

		music = getResource<Music>(music, filename);
#else
		music = getResource<Music>(music, name);
#endif

	    return music;
	}

	TextureRef ResourceManager::getTexture(const std::string& name)
	{
		TextureRef texture;
#ifdef RM_USE_FILENAME_ONLY
		std::string filename = Utils::findFile(name, "assets", true, true);

		if (filename.empty())
		{
		CORE_ERROR("File" + name + "not found");
		static TextureRef empty;
		return empty;
		}

		texture = getResource<Texture>(texture, filename);
#else
		texture = getResource<Texture>(texture, name);
#endif

	    return texture;
	}
}
