#include "Engine/EventSystem.h"

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
			IEvent* firstEvent = m_pEventQueue.front();
			std::type_index eventType = firstEvent->type_index;
			if (!m_pEventHandlersMap.count(eventType))
			{
				m_pEventQueue.pop();
				continue;
			}

			std::vector< std::function<void(IEvent*)>>& handlerList = m_pEventHandlersMap[eventType];

			for (auto handledEvent : handlerList)
			{
				handledEvent(firstEvent);
			}
			m_pEventQueue.pop();
		}
	}

	void EventSystem::queueEvent(IEvent* e)
	{
		m_pEventQueue.push(e);
	}

	void EventSystem::queueEvent(SDL_Event* e)
	{
		uint32_t type = e->type;
		
		switch (type)
		{
		case SDL_WINDOWEVENT:
		{
			WindowEvent* event = new WindowEvent(e->window);
			//event->type_index = typeid(WindowEvent);
			m_pEventQueue.push(event);
		}
			break;
		case SDL_KEYUP:
		case SDL_KEYDOWN:
		{
			KeyEvent* event = new KeyEvent(e->key);
			//event->type_index = typeid(KeyEvent);
			m_pEventQueue.push(event);
		}
			break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		{
			MouseButtonEvent* event = new MouseButtonEvent(e->button);
			//event->type_index = typeid(MouseButtonEvent);
			m_pEventQueue.push(event);
		}
		break;
		case SDL_MOUSEMOTION:
		{
			MouseMovedEvent* event = new MouseMovedEvent(e->motion);
			//event->type_index = typeid(MouseMovedEvent);
			m_pEventQueue.push(event);
		}
		break;
		case SDL_MOUSEWHEEL:
		{
			MouseScrolledEvent* event = new MouseScrolledEvent(e->wheel);
			//event->type_index = typeid(MouseScrolledEvent);
			m_pEventQueue.push(event);
		}
		break;
		case SDL_JOYAXISMOTION:
		{
			JoyAxisEvent* event = new JoyAxisEvent(e->jaxis);
			//event->type_index = typeid(JoyAxisEvent);
			m_pEventQueue.push(event);
		}
		break;
		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
		{
			JoyButtonEvent* event = new JoyButtonEvent(e->jbutton);
			//event->type_index = typeid(JoyButtonEvent);
			m_pEventQueue.push(event);
		}
		break;
		case SDL_JOYDEVICEADDED:
		case SDL_JOYDEVICEREMOVED:
		{
			JoyDeviceEvent* event = new JoyDeviceEvent(e->jdevice);
			//event->type_index = typeid(JoyDeviceEvent);
			m_pEventQueue.push(event);
		}
		break;
		case SDL_CONTROLLERAXISMOTION:
		{
			ControllerAxisEvent* event = new ControllerAxisEvent(e->caxis);
			//event->type_index = typeid(ControllerAxisEvent);
			m_pEventQueue.push(event);
		}
		break;
		case SDL_CONTROLLERBUTTONDOWN:
		case SDL_CONTROLLERBUTTONUP:
		{
			ControllerButtonEvent* event = new ControllerButtonEvent(e->cbutton);
			//event->type_index = typeid(ControllerButtonEvent);
			m_pEventQueue.push(event);
		}
		break;
		case SDL_CONTROLLERDEVICEADDED:
		case SDL_CONTROLLERDEVICEREMOVED:
		{
			ControllerDeviceEvent* event = new ControllerDeviceEvent(e->cdevice);
			//event->type_index = typeid(ControllerDeviceEvent);
			m_pEventQueue.push(event);
		}
		break;
		case SDL_DROPFILE:
		case SDL_DROPTEXT:
		case SDL_DROPBEGIN:
		case SDL_DROPCOMPLETE:
		{
			DropEvent* event = new DropEvent(e->drop);
			//event->type_index = typeid(DropEvent);
			m_pEventQueue.push(event);
		}
		break;
		}
	}

}