#pragma once

#include "Engine/Event.h"

#include <functional>

namespace Engine
{
	class EventHandler
	{
	private:
		std::function<void(IEvent*)> callback;

	public:
		EventHandler(std::function<void(IEvent*)> callback) : callback(callback) {};

		void call(IEvent* e) { callback(e); }
	};
}