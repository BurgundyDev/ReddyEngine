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
#ifdef RM_USE_FILENAME_ONLY
		std::string filename = Utils::findFile(name, "assets", true, true);

		if(filename.empty())
		{
		CORE_ERROR("File " + name + " not found");
		static SoundRef empty;
		return empty;
		}

		return getResource<Sound>(filename);
#else
		return getResource<Sound>(name);
#endif
	}

	MusicRef ResourceManager::getMusic(const std::string& name)
	{
#ifdef RM_USE_FILENAME_ONLY
		std::string filename = Utils::findFile(name, "assets", true, true);

		if (filename.empty())
		{
		CORE_ERROR("File" + name + "not found");
		static MusicRef empty;
		return empty;
		}

		return getResource<Music>(filename);
#else
		return getResource<Music>(name);
#endif
	}

	TextureRef ResourceManager::getTexture(const std::string& name)
	{
#ifdef RM_USE_FILENAME_ONLY
		std::string filename = Utils::findFile(name, "assets", true, true);

		if (filename.empty())
		{
		CORE_ERROR("File" + name + "not found");
		static TextureRef empty;
		return empty;
		}

		return getResource<Texture>(filename);
#else
		return getResource<Texture>(name);
#endif
	}
}
