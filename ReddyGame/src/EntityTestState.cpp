#include "EntityTestState.h"
#include "Game.h"
#include "InGameMenuState.h"
#include "MainMenuState.h"

#include <Engine/ReddyEngine.h>
#include <Engine/EntityManager.h>
#include <Engine/Log.h>

#include <imgui.h>

EntityTestState::EntityTestState()
{

}

void EntityTestState::update(float dt)
{
	if (ImGui::Begin("Entity Spawner", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse))
	{
		if (ImGui::Button("Show Game")) g_pGame->changeState(std::make_shared<InGameMenuState>());
		if (ImGui::Button("Quit")) g_pGame->changeState(std::make_shared<MainMenuState>());

		ImGui::Separator();
		if (ImGui::Button("Spawn Entity"))
		{
			spawnEntity();
		}

		if (ImGui::Button("Spawn Last Entity Child "))
		{
			spawnLastEntityChild();
		}
	}

	ImGui::End();
}

void EntityTestState::spawnEntity()
{
	lastEntity = Engine::getEntityManager()->createEntity();
}

void EntityTestState::spawnLastEntityChild()
{
	if (lastEntity == NULL)
	{
		CLIENT_ERROR("There is no Last Entity!");
		return;
	}
	Engine::getEntityManager()->createEntity(lastEntity);
}