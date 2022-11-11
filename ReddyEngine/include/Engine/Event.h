#pragma once

#include<SDL_events.h>

#include <typeindex>

#define EVENT_TYPE(X)	virtual Engine::EventType GetEventType() const override{ return Engine::EventType::X; }; \
						static Engine::EventType GetStaticEventType() { return Engine::EventType::X; };

// To create new Event you have to:
// 1. add it to the EventTypes
// 2. create new class that inherits from IEvent and use macro EVENT_TYPE(EventType::[your_type])
// 3. create your own field with struct of your event

namespace Engine
{
	enum EventType {
		Window = 0,
		KeyDown,
		KeyUp,
		MouseButton,
		MouseMoved,
		MouseScrolled,
		JoyAxis,
		JoyButton,
		JoyDevice,
		ControllerAxis,
		ControllerButton,
		ControllerDevice,
		Drop,
	};

	class IEvent 
	{
	public:
		IEvent() {};
		virtual EventType GetEventType() const = 0;
	};

	class WindowEvent : public IEvent
	{
	public:
		WindowEvent(SDL_WindowEvent event) : window(event) {};

		SDL_WindowEvent window;

		EVENT_TYPE(Window)
	};

	class KeyDownEvent : public IEvent
	{
	public:
		KeyDownEvent(SDL_KeyboardEvent event) : key(event) {};

		SDL_KeyboardEvent key;
		
		EVENT_TYPE(KeyDown)
	};

	class KeyUpEvent : public IEvent
	{
	public:
		KeyUpEvent(SDL_KeyboardEvent event) : key(event) {};

		SDL_KeyboardEvent key;
		
		EVENT_TYPE(KeyUp)
	};

	class MouseButtonEvent : public IEvent
	{
	public:
		MouseButtonEvent(SDL_MouseButtonEvent event) : button(event) {};

		SDL_MouseButtonEvent button;


		EVENT_TYPE(MouseButton)
	};

	class MouseMovedEvent : public IEvent
	{
	public:
		MouseMovedEvent(SDL_MouseMotionEvent event) : motion(event) {};

		SDL_MouseMotionEvent motion;

		EVENT_TYPE(MouseMoved)
	};

	class MouseScrolledEvent : public IEvent
	{
	public:
		MouseScrolledEvent(SDL_MouseWheelEvent event) : wheel(event) {};
		
		SDL_MouseWheelEvent wheel;
		
		EVENT_TYPE(MouseScrolled)
	};

	class JoyAxisEvent : public IEvent
	{
	public:
		JoyAxisEvent(SDL_JoyAxisEvent event) : jaxis(event) {};

		SDL_JoyAxisEvent jaxis;

		EVENT_TYPE(JoyAxis)
	};

	class JoyButtonEvent : public IEvent
	{
	public:
		JoyButtonEvent(SDL_JoyButtonEvent event) : jbutton(event) {};

		SDL_JoyButtonEvent jbutton;

		EVENT_TYPE(JoyButton)
	};

	class JoyDeviceEvent : public IEvent
	{
	public:
		JoyDeviceEvent(SDL_JoyDeviceEvent event) : jdevice(event) {};

		SDL_JoyDeviceEvent jdevice;

		EVENT_TYPE(JoyDevice)
	};

	class ControllerAxisEvent : public IEvent
	{
	public:
		ControllerAxisEvent(SDL_ControllerAxisEvent event) : caxis(event) {};

		SDL_ControllerAxisEvent caxis;

		EVENT_TYPE(ControllerAxis)
	};

	class ControllerButtonEvent : public IEvent
	{
	public:
		ControllerButtonEvent(SDL_ControllerButtonEvent event) : cbutton(event) {};

		SDL_ControllerButtonEvent cbutton;

		EVENT_TYPE(ControllerButton)
	};

	class ControllerDeviceEvent : public IEvent
	{
	public:
		ControllerDeviceEvent(SDL_ControllerDeviceEvent event) : cdevice(event) {};

		SDL_ControllerDeviceEvent cdevice;

		EVENT_TYPE(ControllerDevice)
	};

	class DropEvent : public IEvent
	{
	public:
		DropEvent(SDL_DropEvent event) : drop(event) {};

		SDL_DropEvent drop;

		EVENT_TYPE(Drop)
	};
};