#pragma once

#include "Engine/Event.h"

#include <functional>
#include <queue>
#include <vector>
#include <map>
#include <typeindex>


using namespace std::placeholders;


namespace Engine
{
	class EventSystem
	{
	public:
		using EventListenerInstance = void*;
	
		struct EventStructure
		{
			IEvent* event;
			std::type_index type;
		};

		struct Listener
		{
			EventListenerInstance pListener;
			std::function<void (IEvent*)> callback;
		};

		EventSystem() {};
		~EventSystem() {};
		
		// Events dispatch

		void dispatchEvents();

		// Event registration

		void sendSDLEvent(SDL_Event* e);

		template<typename T>
		void sendEvent(T* e)
		{
			m_eventQueue.push({e, typeid(T)});
		}

		// Listener registration and deregistration

		template<class T>
		void registerListener(EventListenerInstance instance, const std::function<void (IEvent*)>& callback)
		{

			m_eventHandlersMap[typeid(T)].push_back({instance, callback});
		}

		template<typename T>
		void deregisterListener(EventListenerInstance instance)
		{
			auto& callbacks = m_eventHandlersMap[typeid(T)];

			for (auto it = callbacks.begin(); it != callbacks.end();)
			{
				if (it->pListener == instance)
				{
					it = callbacks.erase(it);
					continue;
				}
				++it;
			}
		}

	private:
		std::queue<EventStructure> m_eventQueue;
		std::map<std::type_index, std::vector<Listener>> m_eventHandlersMap;
		std::vector<Listener> m_listenerCache;
	};
}

