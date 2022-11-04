#include "Engine/Input.h"


namespace Engine
{
    Input::Input()
    {
        memset(m_lastFrameKeyboardState, 0, sizeof(m_lastFrameKeyboardState));
        memset(m_keyboardState, 0, sizeof(m_keyboardState));
    }

    void Input::preUpdate()
    {
        memcpy(m_lastFrameKeyboardState, m_keyboardState, sizeof(m_keyboardState));
    }

    void Input::update()
    {
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

    void Input::onButtonDown(int button)
    {
        //TODO
    }

    void Input::onButtonUp(int button)
    {
        //TODO
    }

    void Input::onMouseMove(const glm::ivec2& pos)
    {
        m_mousePos = {(float)pos.x, (float)pos.y};
    }

    void Input::setMouseMotion(const glm::ivec2& delta)
    {
        m_mouseDelta = {(float)delta.x, (float)delta.y};
    }
}
