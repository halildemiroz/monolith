#pragma once

#include "Registry.h"
#include "Systems.h"
namespace Monolith { class Registry; }

struct Player{
	float speed = 200.0f;
};

void PlayerSystem(Monolith::Registry& Registry);
