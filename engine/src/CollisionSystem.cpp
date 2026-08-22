#include "Components.h"
#include "Entity.h"
#include "EventBus.h"
#include "Events.h"
#include "glm/fwd.hpp"
#include <CollisionSystem.h>
#include <algorithm>
#include <vector>

namespace Monolith {

	CollisionSystem::CollisionSystem(float cellSize) : m_grid(cellSize){}

	std::vector<CollisionPair> CollisionSystem::Update(Registry& reg, EventBus& eventBus){
		m_grid.Clear();
		std::vector<CollisionPair> collisions;
	
		reg.Each<Monolith::Transform, Monolith::BoxCollider>(
				[&](Entity entity, Transform& t, BoxCollider& collider){
				Vec2 center = t.position + collider.offset;
				m_grid.Insert(entity, center, collider.size);
				}
				);
		
		reg.Each<Transform, BoxCollider, Velocity>(
				[&](Entity e, Transform& t, BoxCollider& col, Velocity& v){
				Vec2 centerA = t.position + col.offset;
				auto candidates = m_grid.Query(centerA, col.size, e);

				Vec2 minA = centerA - (col.size * 0.5f);
				Vec2 maxA = centerA + (col.size * 0.5f);

				for(Entity entityB : candidates){
					if(e == entityB)
						continue;
					auto& transformB = reg.Get<Transform>(entityB);
					auto& colB = reg.Get<BoxCollider>(entityB);

					Vec2 centerB = transformB.position + colB.offset;
					Vec2 minB = centerB - (colB.size * 0.5f);
					Vec2 maxB = centerB + (colB.size * 0.5f);

					bool overlap = (minA.x < maxB.x && maxA.x > minB.x && minA.y < maxB.y && maxA.y > minB.y);
					if(overlap){
					collisions.push_back({e, entityB});
					eventBus.Emit(CollisionEvent{e, entityB});
					}
				}
				}
				);

				return collisions;
	}
	
}

