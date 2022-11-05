#pragma once

#include "GameState.h"


// Forward declarations
namespace Engine
{
    class Sound;
    using SoundRef = std::shared_ptr<Sound>;

    class SoundInstance;
    using SoundInstanceRef = std::shared_ptr<SoundInstance>;

    class Music;
    using MusicRef = std::shared_ptr<Music>;
}


class MainMenuState final : public GameState
{
public:
    MainMenuState();
    ~MainMenuState();

    void update(float dt) override;

private:
    Engine::SoundRef m_pQuickSound;

    Engine::SoundRef m_pSound;
    Engine::SoundInstanceRef m_pSoundInstance;
    float m_volume = 1.0f;
    float m_balance = 0.0f;
    float m_pitch = 1.0f;

    Engine::MusicRef m_pMusic;
};
