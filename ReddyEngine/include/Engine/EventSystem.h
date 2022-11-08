#pragma once

#include "Engine/Event.h"

#include <functional>
#include <queue>
#include <set>
#include <map>
#include <typeindex>

using namespace std::placeholders;

namespace Engine
{
	typedef struct EventStructure
	{
		IEvent* event;
		EventType type;
	};


	class EventSystem
	{
	public:
		EventSystem() {};
		~EventSystem() {};
		
		using EventListenerInstance = void*;
	
		// Events dispatch

		void dispatchEvents();

		// Event registration

		template<typename T>
		void registerEvent(T* e)
		{
			m_pEventQueue.push(std::map(e, T::GetType()));
		}

		void registerEvent(SDL_Event* e);


		// Listener registration and deregistration

		template<class T>
		void registerListener(EventListenerInstance instance,  std::function<void (IEvent*)>& callback)
		{
			EventType id = T::GetStaticEventType();
			m_pEventHandlersMap[id].push_back(callback);
		}

		template<typename T>
		void deregisterListener(EventListenerInstance instance)
		{
			std::vector<std::function<void(IEvent*)>>& callbacks = m_pEventHandlersMap[eventType];

			for (auto it = callbacks.begin(); it < it->end(); it++)
			{
				if (it->target == instance)
					callbacks.erase(it);
			}
		}

	private:
		std::queue<EventStructure*> m_pEventQueue;
		std::map<EventType, std::vector<std::function<void(IEvent*)>>> m_pEventHandlersMap;

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

