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
	struct EventStructure
	{
		IEvent* event;
		EventType type;
	};


	class EventSystem
	{
	public:
		using EventListenerInstance = void*;
	
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

		template<typename T>
		void sendEvent(T* e)
		{
			m_pEventQueue.push(std::map(e, T::GetType()));
		}

		void sendEvent(SDL_Event* e);


		// Listener registration and deregistration

		template<class T>
		void registerListener(EventListenerInstance instance, const std::function<void (IEvent*)>& callback)
		{
			EventType id = T::GetStaticEventType();
			m_pEventHandlersMap[id].push_back({instance, callback});
		}

		template<typename T>
		void deregisterListener(EventListenerInstance instance)
		{
			auto& callbacks = m_pEventHandlersMap[T::GetStaticEventType()];

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
		std::queue<EventStructure*> m_pEventQueue;
		std::map<EventType, std::vector<Listener>> m_pEventHandlersMap;

		template<typename T>
		void callEvent(T* e)
		{
			auto handlerList = m_pEventHandlersMap[typeid(T)];
			for (auto callback : handlerList)
			{
				callback(e);
			}
		}

		template <typename T>
		void pushEventToQueue(T* e)
		{
			EventStructure* eventStruct = new EventStructure();
			eventStruct->event = e;
			eventStruct->type = e->GetEventType();

			m_pEventQueue.push(eventStruct);
		}
	};
}

