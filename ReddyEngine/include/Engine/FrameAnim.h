#pragma once

#include <glm/vec2.hpp>
#include <SDL_opengl.h>

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "Resource.h"
#include "Utils.h"

namespace Engine
{
    class FrameAnim;
    using FrameAnimRef = std::shared_ptr<FrameAnim>;
	
    class Texture;
    using TextureRef = std::shared_ptr<Texture>;

	struct Frame
	{
		TextureRef texture;
		float length;
	};

	struct Animation
	{
		std::vector<Frame> frames;
	};

	using AnimationSet = std::unordered_map<std::string, Animation>;

    class FrameAnim : public Resource
    {
    public:
        static FrameAnimRef createFromFile(const std::string& filename);
		static FrameAnimRef createFromJson(const Json::Value& json);

        FrameAnim() = default;
		FrameAnim(const AnimationSet &animationSet)
			: animationSet(animationSet)
		{
		}

		AnimationSet animationSet;
    };
}
