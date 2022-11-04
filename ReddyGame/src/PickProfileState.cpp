#include "PickProfileState.h"
#include "Game.h"
#include "InGameState.h"
#include "MainMenuState.h"

#include <Engine/ReddyEngine.h>

#include <imgui.h>


void PickProfileState::update(float dt)
{
    auto res = Engine::getResolution();
    ImGui::SetNextWindowPos({res.x * 0.5f - 200, res.y * 0.5f - 100});

    if (ImGui::Begin("Pick Profile", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse))
    {
        if (ImGui::Button("Profile 1")) g_pGame->changeState(std::make_shared<InGameState>());
        if (ImGui::Button("Profile 2")) g_pGame->changeState(std::make_shared<InGameState>());
        if (ImGui::Button("Profile 3")) g_pGame->changeState(std::make_shared<InGameState>());
        if (ImGui::Button("<- Back")) g_pGame->changeState(std::make_shared<MainMenuState>());
    }

    ImGui::End();
}
