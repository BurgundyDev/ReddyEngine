#pragma once

#include "GameState.h"

#include <glm/vec2.hpp>
#include <string>
#include <SDL_events.h>


// Forward declarations
namespace Engine
{
    class Sound;
    using SoundRef = std::shared_ptr<Sound>;

    class SoundInstance;
    using SoundInstanceRef = std::shared_ptr<SoundInstance>;

    class Music;
    using MusicRef = std::shared_ptr<Music>;

    class Font;
    using FontRef = std::shared_ptr<Font>;
}


class MainMenuState final : public GameState
{
public:
    MainMenuState();
    ~MainMenuState();

    void update(float dt) override;
    void draw() override;

    void enter(const GameStateRef& previousState) override;

private:
    Engine::SoundRef m_pQuickSound;

    Engine::SoundRef m_pSound;
    Engine::SoundInstanceRef m_pSoundInstance;
    float m_volume = 1.0f;
    float m_balance = 0.0f;
    float m_pitch = 1.0f;

    Engine::MusicRef m_pMusic;

    Engine::FontRef m_pFont;
    float m_textRotation = 0.0f;
    float m_textScale = 1.0f;
    glm::vec2 m_textAlign = { 0.5f, 0.5f };
};
