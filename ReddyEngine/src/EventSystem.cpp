#include "Engine/EventSystem.h"
#include "Engine/Log.h"

namespace Engine
{
	void EventSystem::sendSDLEvent(SDL_Event* e)
	{
		switch (e->type)
		{
			case SDL_WINDOWEVENT:
				sendEvent(new WindowEvent(e->window));
				break;

			case SDL_KEYUP:
				sendEvent(new KeyUpEvent(e->key));
				break;

			case SDL_KEYDOWN:
				sendEvent(new KeyDownEvent(e->key));
				break;

			case SDL_MOUSEBUTTONDOWN:
				sendEvent(new MouseButtonDownEvent(e->button));
				break;

			case SDL_MOUSEBUTTONUP:
				sendEvent<MouseButtonUpEvent>(new MouseButtonUpEvent(e->button));
				break;

			case SDL_MOUSEMOTION:
				sendEvent(new MouseMovedEvent(e->motion));
				break;

			case SDL_MOUSEWHEEL:
				sendEvent(new MouseScrolledEvent(e->wheel));
				break;

			case SDL_JOYAXISMOTION:
				sendEvent(new JoyAxisEvent(e->jaxis));
				break;

			case SDL_JOYBUTTONDOWN:
			case SDL_JOYBUTTONUP:
				sendEvent(new JoyButtonEvent(e->jbutton));
				break;

			case SDL_JOYDEVICEADDED:
			case SDL_JOYDEVICEREMOVED:
				sendEvent(new JoyDeviceEvent(e->jdevice));
				break;

			case SDL_CONTROLLERAXISMOTION:
				sendEvent(new ControllerAxisEvent(e->caxis));
				break;

			case SDL_CONTROLLERBUTTONDOWN:
			case SDL_CONTROLLERBUTTONUP:
				sendEvent(new ControllerButtonEvent(e->cbutton));
				break;

			case SDL_CONTROLLERDEVICEADDED:
			case SDL_CONTROLLERDEVICEREMOVED:
				sendEvent(new ControllerDeviceEvent(e->cdevice));
				break;

			case SDL_DROPFILE:
			case SDL_DROPTEXT:
			case SDL_DROPBEGIN:
			case SDL_DROPCOMPLETE:
				sendEvent(new DropEvent(e->drop));
				break;
		}
	}

	void EventSystem::dispatchEvents()
	{
		while (!m_eventQueue.empty())
		{
			auto eventStructure = m_eventQueue.front();
			m_eventQueue.pop();

			if (eventStructure.type == typeid(LuaEvent))
			{
				auto pLuaEvent = (LuaEvent*)eventStructure.event;
				m_listenerCache = m_luaEventHandlersMap[pLuaEvent->name]; // There's a risk here that an event adds/removes to the listeners while we're iterating it
				for (auto kv : m_listenerCache)
				{
					eventStructure.event->pListener = kv.pListener;
					kv.callback(eventStructure.event);
				}
				m_listenerCache.clear();
			}
			else
			{
				m_listenerCache = m_eventHandlersMap[eventStructure.type]; // There's a risk here that an event adds/removes to the listeners while we're iterating it
				for (auto kv : m_listenerCache)
				{
					eventStructure.event->pListener = kv.pListener;
					kv.callback(eventStructure.event);
				}
				m_listenerCache.clear();
			}

			delete eventStructure.event;
		}
	}
}
