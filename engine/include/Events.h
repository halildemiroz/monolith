#pragma once

#include "Entity.h"
namespace Monolith {
	
	struct WindowResizeEvent{
		int width = 0;
		int height = 0;
	};

	struct CollisionEvent{
		Entity a, b;
	};

	struct WindowCloseEvent{};
	
}
