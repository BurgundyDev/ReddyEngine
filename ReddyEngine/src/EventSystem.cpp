#include "Engine/EventSystem.h"
#include "Engine/Log.h"

namespace Engine
{
	void EventSystem::registerEvent(SDL_Event* e)
	{
		switch (e->type)
		{
		case SDL_WINDOWEVENT:
		{
			WindowEvent* event = new WindowEvent(e->window);
			pushEventToQueue<WindowEvent>(event);
		}
		break;
		case SDL_KEYUP:
		case SDL_KEYDOWN:
		{
			KeyEvent* event = new KeyEvent(e->key);
			pushEventToQueue<KeyEvent>(event);
		}
		break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		{
			MouseButtonEvent* event = new MouseButtonEvent(e->button);
			pushEventToQueue<MouseButtonEvent>(event);
		}
		break;
		case SDL_MOUSEMOTION:
		{
			MouseMovedEvent* event = new MouseMovedEvent(e->motion);
			pushEventToQueue<MouseMovedEvent>(event);
		}
		break;
		case SDL_MOUSEWHEEL:
		{
			MouseScrolledEvent* event = new MouseScrolledEvent(e->wheel);
			pushEventToQueue<MouseScrolledEvent>(event);
		}
		break;
		case SDL_JOYAXISMOTION:
		{
			JoyAxisEvent* event = new JoyAxisEvent(e->jaxis);
			pushEventToQueue<JoyAxisEvent>(event);
		}
		break;
		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
		{
			JoyButtonEvent* event = new JoyButtonEvent(e->jbutton);
			pushEventToQueue<JoyButtonEvent>(event);
		}
		break;
		case SDL_JOYDEVICEADDED:
		case SDL_JOYDEVICEREMOVED:
		{
			JoyDeviceEvent* event = new JoyDeviceEvent(e->jdevice);
			pushEventToQueue<JoyDeviceEvent>(event);
		}
		break;
		case SDL_CONTROLLERAXISMOTION:
		{
			ControllerAxisEvent* event = new ControllerAxisEvent(e->caxis);
			pushEventToQueue<ControllerAxisEvent>(event);
		}
		break;
		case SDL_CONTROLLERBUTTONDOWN:
		case SDL_CONTROLLERBUTTONUP:
		{
			ControllerButtonEvent* event = new ControllerButtonEvent(e->cbutton);
			pushEventToQueue<ControllerButtonEvent>(event);
		}
		break;
		case SDL_CONTROLLERDEVICEADDED:
		case SDL_CONTROLLERDEVICEREMOVED:
		{
			ControllerDeviceEvent* event = new ControllerDeviceEvent(e->cdevice);
			pushEventToQueue<ControllerDeviceEvent>(event);
		}
		break;
		case SDL_DROPFILE:
		case SDL_DROPTEXT:
		case SDL_DROPBEGIN:
		case SDL_DROPCOMPLETE:
		{
			DropEvent* event = new DropEvent(e->drop);
			pushEventToQueue<DropEvent>(event);
		}
		break;
		}
	}

	void EventSystem::dispatchEvents()
	{
		while (!m_pEventQueue.empty())
		{
			IEvent* e = m_pEventQueue.front()->event;
			EventType type = m_pEventQueue.front()->type;

			auto listeners = m_pEventHandlersMap[type];
			for (auto callback : listeners)
			{
				callback(e);
			}
			m_pEventQueue.pop();
		}
	}

}