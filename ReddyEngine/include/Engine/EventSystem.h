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
	private:
		typedef void* EventListenerInstance;

		std::queue<EventStructure*> m_pEventQueue;
		std::map<EventType, std::vector<std::function<void(IEvent*)>>> m_pEventHandlersMap;
	public:
		EventSystem(); // remove later
		~EventSystem();

		template <typename T> 
		void pushEventToQueue(T* e);
	
		template<typename T>
		void registerEvent(T* e)
		{
			m_pEventQueue.push(std::map(e, T::GetType()));
		}

		void dispatchEvents();

		void registerEvent(SDL_Event* e);

		template<class T>
		void registerListener(EventListenerInstance instance,  std::function<void (IEvent*)>& callback)
		{
			EventType id = T::GetStaticEventType();
			//if (m_pEventHandlersMap[id].size() == 0)
			//	m_pEventHandlersMap[id] = std::vector<T*>;

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
		template<typename T>
		void callEvent(T* e)
		{
			auto handlerList = m_pEventHandlersMap[typeid(T)];
			for (auto callback : handlerList)
			{
				callback(e);
			}
		}
	};
}

