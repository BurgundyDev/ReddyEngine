#include "Engine/EventSystem.h"

#include "Engine/Log.h"

namespace Engine
{
	EventSystem::EventSystem()
	{
	}

	EventSystem::~EventSystem()
	{

	}

	void EventSystem::dispatchEvents()
	{
		while (!m_pEventQueue.empty())
		{
			SDL_Event* firstEvent = &m_pEventQueue.front();
			SDL_EventType eventType = (SDL_EventType)firstEvent->type;

			if (!m_pEventHandlersMap.count(eventType))
			{
				m_pEventQueue.pop();
				continue;
			}

			HandlerList* handlerList = m_pEventHandlersMap[eventType];

			for (auto handledEvent : *handlerList)
			{
				handledEvent->exec(firstEvent);
			}
			m_pEventQueue.pop();
		}
	}

	void EventSystem::queueEvent(SDL_Event* e)
	{
		m_pEventQueue.push(*e);
	}

}