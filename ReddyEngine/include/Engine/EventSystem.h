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
	class EventSystem
	{
	private:
		typedef void* EventHandler;

		std::queue<IEvent*> m_pEventQueue;
		std::map<std::type_index, std::vector<std::function<void(IEvent*)>>> m_pEventHandlersMap;
	public:
		EventSystem(); // remove later
		~EventSystem();

		void dispatchEvents();

		template <typename T>
		void queueEvent(Event<T>* e)
		{
			m_pEventQueue.push(e);
		}
		void queueEvent(SDL_Event* e);

		template<typename T>
		void registerListener(EventHandler instance,  std::function<void (IEvent*)> callback)
		{
			std::type_index id = typeid(T);
			//if (m_pEventHandlersMap[id].size() == 0)
			//	m_pEventHandlersMap[id] = std::vector<T*>;

			m_pEventHandlersMap[id].push_back(callback);
		}

		template<typename T>
		void deregisterListener(EventHandler instance)
		{
			std::vector<std::function<void(IEvent*)>>& callbacks = m_pEventHandlersMap[eventType];

			for (auto it = callbacks.begin(); it < it->end(); it++)
			{
				if (it->target == instance)
					callbacks.erase(it);
			}
		}
	};
}

