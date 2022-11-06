#include "MainMenuState.h"
#include "EditorState.h"
#include "Game.h"
#include "PickProfileState.h"

#include <Engine/Font.h>
#include <Engine/Music.h>
#include <Engine/ReddyEngine.h>
#include <Engine/Sound.h>
#include <Engine/SpriteBatch.h>

#include <imgui.h>

Engine::ResourceManagerRef g_pResourceManager;

MainMenuState::MainMenuState()
{
    m_pQuickSound = g_pResourceManager->getSound("test_quick_sound.wav");

    // For a sound where we need to keep instance because it loops and we want to control it's volume/panning over its lifetime.
    m_pSound = g_pResourceManager->getSound("test_sound.wav");
    m_pSoundInstance = std::make_shared<Engine::SoundInstance>(m_pSound);
    m_pSoundInstance->setLoop(true);

    // m_pMusic = Engine::Music::createFromFile("assets/musics/test_music.ogg");
    m_pMusic = g_pResourceManager->getMusic("test_music.ogg");

    m_pFont = Engine::Font::createFromFile("assets/fonts/test_font.otf", 24);
}

MainMenuState::~MainMenuState()
{
}

void MainMenuState::update(float dt)
{
    auto res = Engine::getResolution();
    ImGui::SetNextWindowPos({res.x * 0.5f - 200, res.y * 0.5f - 100});

    if (ImGui::Begin("Main Menu", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse))
    {
        ImGui::Text("Using ImGui temporarly, we don't have UIs yet.");
		if (ImGui::Button("Play")) g_pGame->changeState(std::make_shared<PickProfileState>());
        if (ImGui::Button("Quit")) Engine::quit();

        ImGui::Separator();
        if (ImGui::Button("Editor")) g_pGame->changeState(std::make_shared<EditorState>());

        ImGui::Separator();
        if (ImGui::Button("Play \"Fire and Forget\" Sound")) m_pQuickSound->play();

        if (ImGui::Button("Play/Stop looping Sound"))
        {
            if (m_pSoundInstance->isPlaying()) m_pSoundInstance->stop();
            else m_pSoundInstance->play();
        }

        if (ImGui::SliderFloat("Volume", &m_volume, 0.0f, 1.0f)) m_pSoundInstance->setVolume(m_volume);
        if (ImGui::SliderFloat("Balance", &m_balance, -1.0f, 1.0f)) m_pSoundInstance->setBalance(m_balance);
        if (ImGui::SliderFloat("Pitch", &m_pitch, 0.1f, 10.0f)) m_pSoundInstance->setPitch(m_pitch);

        if (ImGui::Button("Play/Stop music"))
        {
            if (m_pMusic->isPlaying()) m_pMusic->stop();
            else m_pMusic->play();
        }

        ImGui::SliderFloat("Text Rotation", &m_textRotation, -180.0f, 180.0f);
        ImGui::SliderFloat("Text Scale", &m_textScale, -0.1f, 10.0f);
        ImGui::SliderFloat2("Text Align", &m_textAlign.x, 0.0f, 1.0f);
    }

    ImGui::End();
}

void MainMenuState::draw()
{
    auto sb = Engine::getSpriteBatch().get();
    auto res = Engine::getResolution();

    sb->begin();
    if (m_pFont) m_pFont->draw("The quick brown fox jumps over the lazy dog.", {res.x * 0.5f, 100.0f}, {1, 1, 1, 1}, m_textRotation, m_textScale, m_textAlign);
    sb->end();
}
