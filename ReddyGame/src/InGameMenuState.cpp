#include "InGameMenuState.h"
#include "Game.h"
#include "MainMenuState.h"

#include <Engine/Input.h>
#include <Engine/ReddyEngine.h>

#include <imgui.h>


InGameMenuState::InGameMenuState()
{
    m_seeThrough = true;
}


void InGameMenuState::update(float dt)
{
    if (Engine::getInput()->isKeyJustDown(SDL_SCANCODE_ESCAPE))
    {
        g_pGame->popState();
        return;
    }


    auto res = Engine::getResolution();
    ImGui::SetNextWindowPos({res.x * 0.5f - 200, res.y * 0.5f - 100});

    if (ImGui::Begin("In Game Menu", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse))
    {
        if (ImGui::Button("Resume")) g_pGame->popState();
        if (ImGui::Button("Quit")) g_pGame->changeState(std::make_shared<MainMenuState>());
    }

    ImGui::End();
}
