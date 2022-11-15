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

		void registerLuaListener(const std::string& eventName, EventListenerInstance instance, const std::function<void (IEvent*)>& callback)
		{
			m_luaEventHandlersMap[eventName].push_back({instance, callback});
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

		void deregisterLuaListener(const std::string& eventName, EventListenerInstance instance)
		{
			auto& callbacks = m_luaEventHandlersMap[eventName];

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
		std::map<std::string, std::vector<Listener>> m_luaEventHandlersMap;
		std::vector<Listener> m_listenerCache;
	};
}

// Shortcuts to make life easier
#define REGISTER_EVENT(eventType, function) Engine::getEventSystem()->registerListener<Engine::eventType>(this, std::bind(&function, this, _1))
#define DEREGISTER_EVENT(eventType) Engine::getEventSystem()->deregisterListener<Engine::eventType>(this)
