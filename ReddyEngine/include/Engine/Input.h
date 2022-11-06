#pragma once

#include <glm/vec2.hpp>

#include <memory>

#include <SDL_keycode.h>
#include <SDL_mouse.h>


namespace Engine
{
    class Input
    {
    public:
        Input();

        void preUpdate(); // Before SDL events processing
        void update(); // After SDL events processing

        const glm::vec2& getMousePos() const { return m_mousePos; }
        const glm::vec2& getMouseDelta() const { return m_mouseDelta; }
        int getMouseWheel() const { return m_mouseWheel; }

        bool isKeyDown(SDL_Scancode scancode) const;
        bool isKeyUp(SDL_Scancode scancode) const;
        bool isKeyJustDown(SDL_Scancode scancode) const; // Was pressed this frame
        bool isKeyJustUp(SDL_Scancode scanecode) const; // Was released this frame

        bool isButtonDown(int button) const;
        bool isButtonUp(int button) const;
        bool isButtonJustDown(int button) const; // Was pressed this frame
        bool isButtonJustUp(int button) const; // Was released this frame

        void onKeyDown(SDL_Scancode scancode);
        void onKeyUp(SDL_Scancode scancode);
        void onButtonDown(int button);
        void onButtonUp(int button);
        void onMouseMove(const glm::ivec2& pos);

        void setMouseMotion(const glm::ivec2& delta);
        void setMouseWheelMotion(int vel);

    private:
        glm::vec2 m_mousePos = {0, 0};
        glm::vec2 m_mouseDelta = {0, 0};
        int m_mouseWheel = 0;

        bool m_lastFrameKeyboardState[SDL_NUM_SCANCODES];
        bool m_keyboardState[SDL_NUM_SCANCODES];
        bool m_lastButtonState[8];
        bool m_buttonState[8];
    };


    using InputRef = std::shared_ptr<Input>;
}
