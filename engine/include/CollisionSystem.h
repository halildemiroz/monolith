#pragma once
#include "Entity.h"
#include "EventBus.h"
#include "Systems.h"
#include <Registry.h>
#include <SpatialGrid.h>

namespace Monolith {

	struct CollisionPair{
		Entity a;
		Entity b;
	};

	class CollisionSystem{
		public:
			explicit CollisionSystem(float cellSize = 64.0f);
			std::vector<CollisionPair> Update(Registry& reg, EventBus& eventBus);

		private:
			SpatialGrid m_grid;
	};

}
