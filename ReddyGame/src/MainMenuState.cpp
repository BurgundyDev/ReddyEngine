#include "MainMenuState.h"
#include "EditorState.h"
#include "Game.h"
#include "InGameState.h"

#include <Engine/ReddyEngine.h>
#include <Engine/Input.h>
#include <Engine/Utils.h>

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
        if (ImGui::Button("Continue")) g_pGame->changeState(std::make_shared<InGameState>(Engine::Utils::getSavePath("Reddy") + "world.json"));
        if (ImGui::Button("New Game")) g_pGame->changeState(std::make_shared<InGameState>("assets/scenes/world.json"));
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
    //m_saveExists = Utils::fileExist(Engine::Utils::getSavePath("Reddy") + "world.json");
    Engine::getInput()->setDefaultCursor();
}
