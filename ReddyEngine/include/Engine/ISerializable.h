#pragma once

#include "json/value.h"

namespace Engine
{
	class ISerializable
	{
	public:
		virtual Json::Value serialize() = 0;
		virtual void deserialize(Json::Value json) = 0;
	};
}
