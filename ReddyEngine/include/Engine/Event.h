#pragma once

#include<SDL_events.h>

#include <typeindex>

namespace Engine
{
	class IEvent
	{
	public:
		std::type_index type_index;

		IEvent() : type_index(typeid(int)) {};
	};

	template <typename T>
	class Event : public IEvent
	{
	public:
		std::type_index type_index;

		Event() : type_index(typeid(T)) {};
	};

	class WindowEvent : public Event<WindowEvent>
	{
	public:
		WindowEvent(SDL_WindowEvent event) : window(event) {};

		SDL_WindowEvent window;
	};

	class KeyEvent : public Event<KeyEvent>
	{
	public:
		KeyEvent(SDL_KeyboardEvent event) : key(event) {};

		SDL_KeyboardEvent key;
	};

	class MouseButtonEvent : public Event<MouseButtonEvent>
	{
	public:
		MouseButtonEvent(SDL_MouseButtonEvent event) : button(event) {};

		SDL_MouseButtonEvent button;
	};

	class MouseMovedEvent : public Event<MouseMovedEvent>
	{
	public:
		MouseMovedEvent(SDL_MouseMotionEvent event) : motion(event) {};

		SDL_MouseMotionEvent motion;
	};

	class MouseScrolledEvent : public Event<MouseScrolledEvent>
	{
	public:
		MouseScrolledEvent(SDL_MouseWheelEvent event) : wheel(event) {};
		
		SDL_MouseWheelEvent wheel;
	};

	class JoyAxisEvent : public Event<JoyAxisEvent>
	{
	public:
		JoyAxisEvent(SDL_JoyAxisEvent event) : jaxis(event) {};

		SDL_JoyAxisEvent jaxis;
	};

	class JoyButtonEvent : public Event<JoyButtonEvent>
	{
	public:
		JoyButtonEvent(SDL_JoyButtonEvent event) : jbutton(event) {};

		SDL_JoyButtonEvent jbutton;
	};

	class JoyDeviceEvent : public Event<JoyDeviceEvent>
	{
	public:
		JoyDeviceEvent(SDL_JoyDeviceEvent event) : jdevice(event) {};

		SDL_JoyDeviceEvent jdevice;
	};

	class ControllerAxisEvent : public Event<ControllerAxisEvent>
	{
	public:
		ControllerAxisEvent(SDL_ControllerAxisEvent event) : caxis(event) {};

		SDL_ControllerAxisEvent caxis;
	};

	class ControllerButtonEvent : public Event<ControllerButtonEvent>
	{
	public:
		ControllerButtonEvent(SDL_ControllerButtonEvent event) : cbutton(event) {};

		SDL_ControllerButtonEvent cbutton;
	};

	class ControllerDeviceEvent : public Event<ControllerDeviceEvent>
	{
	public:
		ControllerDeviceEvent(SDL_ControllerDeviceEvent event) : cdevice(event) {};

		SDL_ControllerDeviceEvent cdevice;
	};

	class DropEvent : public Event<DropEvent>
	{
	public:
		DropEvent(SDL_DropEvent event) : drop(event) {};

		SDL_DropEvent drop;
	};
};