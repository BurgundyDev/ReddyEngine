#pragma once

#include<SDL_events.h>

#include <typeindex>

namespace Engine
{
	class IEvent
	{
	public:
		std::type_index type_index;
		
		IEvent() : type_index(typeid(this)) {};
	};

	class WindowEvent : public IEvent
	{
	public:
		WindowEvent(SDL_WindowEvent event) : window(event) {};

		SDL_WindowEvent window;
	};

	class KeyEvent : public IEvent
	{
	public:
		KeyEvent(SDL_KeyboardEvent event) : key(event) {};

		SDL_KeyboardEvent key;
	};

	class MouseButtonEvent : public IEvent
	{
	public:
		MouseButtonEvent(SDL_MouseButtonEvent event) : button(event) {};

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

		SDL_DropEvent drop;
	};
};