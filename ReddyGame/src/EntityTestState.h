#pragma once

#include "GameState.h"

#include <Engine/EntityManager.h>

class EntityTestState final : public GameState
{
public:
	EntityTestState();

	void update(float dt) override;

	void spawnEntity();
	void spawnLastEntityChild();
private:
	Engine::EntityRef lastEntity = NULL;
};
