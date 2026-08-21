#pragma once

#include "Registry.h"
#include "Systems.h"
namespace Monolith { class Registry; }

struct Player{
	float speed;
};

void PlayerSystem(Monolith::Registry& Registry);
