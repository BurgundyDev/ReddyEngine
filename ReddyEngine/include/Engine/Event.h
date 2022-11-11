#pragma once

#include<SDL_events.h>

#include <typeindex>


namespace Engine
{
	class IEvent 
	{
	public:
		IEvent() {};
		IEvent(const IEvent &other) = delete;
		IEvent &operator=(const IEvent &other) = delete;
		virtual ~IEvent() = default;
	};

	class WindowEvent : public IEvent
	{
	public:
		WindowEvent(SDL_WindowEvent event) : window(event) {};

		SDL_WindowEvent window;
	};

	class KeyDownEvent : public IEvent
	{
	public:
		KeyDownEvent(SDL_KeyboardEvent event) : key(event) {};

		SDL_KeyboardEvent key;
	};

	class KeyUpEvent : public IEvent
	{
	public:
		KeyUpEvent(SDL_KeyboardEvent event) : key(event) {};

		SDL_KeyboardEvent key;
	};

	class MouseButtonUpEvent : public IEvent
	{
	public:
		MouseButtonUpEvent(SDL_MouseButtonEvent event) : button(event) {};

		SDL_MouseButtonEvent button;
	};

	class MouseButtonDownEvent : public IEvent
	{
	public:
		MouseButtonDownEvent(SDL_MouseButtonEvent event) : button(event) {};

		SDL_MouseButtonEvent button;
	};

	class MouseMovedEvent : public IEvent
	{
	public:
		MouseMovedEvent(SDL_MouseMotionEvent event) : motion(event) {};

		SDL_MouseMotionEvent motion;
	};

	class MouseScrolledEvent : public IEvent
	{
	public:
		MouseScrolledEvent(SDL_MouseWheelEvent event) : wheel(event) {};
		
		SDL_MouseWheelEvent wheel;
	};

	class JoyAxisEvent : public IEvent
	{
	public:
		JoyAxisEvent(SDL_JoyAxisEvent event) : jaxis(event) {};

		SDL_JoyAxisEvent jaxis;
	};

	class JoyButtonEvent : public IEvent
	{
	public:
		JoyButtonEvent(SDL_JoyButtonEvent event) : jbutton(event) {};

		SDL_JoyButtonEvent jbutton;
	};

	class JoyDeviceEvent : public IEvent
	{
	public:
		JoyDeviceEvent(SDL_JoyDeviceEvent event) : jdevice(event) {};

		SDL_JoyDeviceEvent jdevice;
	};

	class ControllerAxisEvent : public IEvent
	{
	public:
		ControllerAxisEvent(SDL_ControllerAxisEvent event) : caxis(event) {};

		SDL_ControllerAxisEvent caxis;
	};

	class ControllerButtonEvent : public IEvent
	{
	public:
		ControllerButtonEvent(SDL_ControllerButtonEvent event) : cbutton(event) {};

		SDL_ControllerButtonEvent cbutton;
	};

	class ControllerDeviceEvent : public IEvent
	{
	public:
		ControllerDeviceEvent(SDL_ControllerDeviceEvent event) : cdevice(event) {};

		SDL_ControllerDeviceEvent cdevice;
	};

	class DropEvent : public IEvent
	{
	public:
		DropEvent(SDL_DropEvent event) : drop(event) {};

		DropEvent(const DropEvent &other) = delete;
		DropEvent &operator=(const DropEvent &other) = delete;

		// make sure to use SDL_free on file name ptr!
		virtual ~DropEvent() override
		{
			if (drop.file) {
				SDL_free(drop.file);
			}
		}

		SDL_DropEvent drop;
	};
};