#include "MainMenuState.h"
#include "EditorState.h"
#include "Game.h"
#include "PickProfileState.h"

#include <Engine/Font.h>
#include <Engine/Music.h>
#include <Engine/ReddyEngine.h>
#include <Engine/Sound.h>
#include <Engine/SpriteBatch.h>
#include <Engine/Input.h>

#include <imgui.h>
#include <functional>


MainMenuState::MainMenuState()
{
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
    }

    ImGui::End();
}

void MainMenuState::draw()
{
}

void MainMenuState::enter(const GameStateRef& previousState)
{
    Engine::getInput()->setDefaultCursor();
}

