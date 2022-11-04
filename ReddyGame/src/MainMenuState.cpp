#include "MainMenuState.h"
#include "Game.h"
#include "PickProfileState.h"

#include <Engine/ReddyEngine.h>

#include <imgui.h>


void MainMenuState::update(float dt)
{
    auto res = Engine::getResolution();
    ImGui::SetNextWindowPos({res.x * 0.5f - 200, res.y * 0.5f - 100});

    if (ImGui::Begin("Main Menu", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse))
    {
        ImGui::Text("Using ImGui temporarly, we don't have UIs yet.");
        if (ImGui::Button("Play")) g_pGame->changeState(std::make_shared<PickProfileState>());
        if (ImGui::Button("Quit")) Engine::quit();
    }

    ImGui::End();
}
