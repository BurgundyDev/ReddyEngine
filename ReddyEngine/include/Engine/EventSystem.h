#pragma once

#include <SDL_events.h>

#include <functional>
#include <queue>
#include <set>
#include <map>

namespace Engine
{
	class EventHandlerBase {
	public:
		// Call the member function
		void exec(SDL_Event* evnt) {
			call(evnt);
		}
	private:
		// Implemented by MemberFunctionHandler
		virtual void call(SDL_Event* evnt) = 0;
	};

	template<class T>
	class EventHandler : public EventHandlerBase
	{
	public:
		EventHandler(T* instance, std::function<void(SDL_Event*)> callback) : instance(instance),
			callback(callback) {};

		void call(SDL_Event* e)
		{
			callback(e);
		}

	private:
		T* instance;
		std::function<void(SDL_Event*)> callback;
	};


	typedef std::list<EventHandlerBase*> HandlerList;

	class EventSystem
	{
	private:
		std::queue<SDL_Event> m_pEventQueue;
		std::map<SDL_EventType, HandlerList*> m_pEventHandlersMap;
	public:
		EventSystem(); // check if it will be useful later
		~EventSystem();

		void dispatchEvents();
		void queueEvent(SDL_Event* e);

		template<class T>
		void registerListener(SDL_EventType eventType, T* instance,  std::function<void(SDL_Event*)> callback)
		{
			if (m_pEventHandlersMap[eventType] == nullptr)
				m_pEventHandlersMap[eventType] = new HandlerList();

			m_pEventHandlersMap[eventType]->push_back(
				new EventHandler<T>(instance, callback)
			);
		}

		template<class T, class EventType>
		void deregisterListener(EventType eventType, T* instance)
		{
			HandlerList* callbacks = m_pEventHandlersMap[eventType];

			for (auto it = callbacks.begin(); it < it->end(); it++)
			{
				if (it->target == instance)
				{
					callbacks.erase(it);
				}
			}
		}
	};
}

