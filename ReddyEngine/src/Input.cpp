#include "Engine/Input.h"

#include "Engine/Texture.h"
#include "imgui.h"
#include <stb_image.h>

namespace Engine
{
    Input::Input()
    {
        memset(m_lastFrameKeyboardState, 0, sizeof(m_lastFrameKeyboardState));
        memset(m_keyboardState, 0, sizeof(m_keyboardState));
        memset(m_lastButtonState, 0, sizeof(m_lastButtonState));
        memset(m_buttonState, 0, sizeof(m_buttonState));
    }

    void Input::preUpdate()
    {
        memcpy(m_lastFrameKeyboardState, m_keyboardState, sizeof(m_keyboardState));
        memcpy(m_lastButtonState, m_buttonState, sizeof(m_buttonState));
        m_mouseDelta = {0, 0};
        m_mouseWheel = 0;
    }

    void Input::update()
    {
    }

	void Input::keyEventCallback(IEvent* e)
	{
        KeyEvent* ke = (KeyEvent*) e;

        switch (ke->key.type)
        {
		case SDL_KEYDOWN:
            onKeyDown(ke->key.keysym.scancode);
            break;
		case SDL_KEYUP:
            onKeyUp(ke->key.keysym.scancode);
			break;
		}
	}

	bool Input::isKeyDown(SDL_Scancode scancode) const
    {
        return m_keyboardState[(int)scancode];
    }

    bool Input::isKeyUp(SDL_Scancode scancode) const
    {
        return !m_keyboardState[(int)scancode];
    }

    bool Input::isKeyJustDown(SDL_Scancode scancode) const
    {
        // Issue with that is if someone clicks real fast, he could get down/up events inside the same frame.
        // So it's better to use events for this. We can use some tricks with frame Id but we should just
        // use events and move on.
        return !m_lastFrameKeyboardState[(int)scancode] &&
                m_keyboardState[(int)scancode];
    }

    bool Input::isKeyJustUp(SDL_Scancode scancode) const
    {
        return  m_lastFrameKeyboardState[(int)scancode] &&
               !m_keyboardState[(int)scancode];
    }

    void Input::onKeyDown(SDL_Scancode scancode)
    {
        m_keyboardState[(int)scancode] = true;
    }

    void Input::onKeyUp(SDL_Scancode scancode)
    {
        m_keyboardState[(int)scancode] = false;
    }

    bool Input::isButtonDown(int button) const
    {
        return m_buttonState[button];
    }

    bool Input::isButtonUp(int button) const
    {
        return !m_buttonState[button];
    }

    bool Input::isButtonJustDown(int button) const
    {
        return !m_lastButtonState[button] &&
                m_buttonState[button];
    }

    bool Input::isButtonJustUp(int button) const
    {
        return  m_lastButtonState[button] &&
               !m_buttonState[button];
    }

    void Input::onButtonDown(int button)
    {
        m_buttonState[button] = true;
    }

    void Input::onButtonUp(int button)
    {
        m_buttonState[button] = false;
    }

    void Input::onMouseMove(const glm::ivec2& pos)
    {
        m_mousePos = {(float)pos.x, (float)pos.y};
    }

    void Input::setMouseMotion(const glm::ivec2& delta)
    {
        m_mouseDelta = {(float)delta.x, (float)delta.y};
    }

    void Input::setMouseWheelMotion(int vel)
    {
        m_mouseWheel = vel;
    }

	void Input::setMouseCursor(const std::string& path, glm::vec2 hotSpot)
	{
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

        if (m_cursors[path] == nullptr)
        {
            int pSizeX, pSizeY, channels;
            auto cursorImg = stbi_load(path.c_str(), &pSizeX, &pSizeY, &channels, 4);
            

			auto pSurface = SDL_CreateRGBSurfaceFrom(cursorImg,
                pSizeX,
                pSizeY,
				32,
                pSizeX * 4,
				0x000000ff,
				0x0000ff00,
				0x00ff0000,
				0xff000000);

            if (pSurface)
            {
                SDL_Cursor* pCursor = SDL_CreateColorCursor(pSurface, hotSpot.x, hotSpot.y);
                if (pCursor)
                    m_cursors[path] = pCursor;
            }
			SDL_FreeSurface(pSurface);
        }

        SDL_Cursor* cursor = m_cursors[path];
		if (cursor)
		{
			SDL_SetCursor(cursor);
		}

        m_isCursorCustomSet = true;
	}

	void Input::setSystemMouseCursor(SDL_SystemCursor pSysCursorType)
	{
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

        auto cursor = SDL_CreateSystemCursor(pSysCursorType);
		
        if (cursor)
		{
			SDL_SetCursor(cursor);
		}

		m_isCursorCustomSet = true;
	}

	void Input::setDefaultCursor()
	{
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
		m_isCursorCustomSet = false;
	}

}
