#include "Components.h"
#include "Entity.h"
#include "glm/fwd.hpp"
#include <CollisionSystem.h>
#include <algorithm>
#include <vector>

namespace Monolith {

	CollisionSystem::CollisionSystem(float cellSize){}

	std::vector<CollisionPair> CollisionSystem::Update(Registry& reg){
		m_grid.Clear();
		std::vector<CollisionPair> collisions;
	
		reg.Each<Monolith::Transform, Monolith::BoxCollider>(
				[&](Entity entity, Transform& t, BoxCollider& collider){
				glm::vec2 center = t.position + collider.offset;
				m_grid.Insert(entity, center, collider.size);
				}
				);
		
		reg.Each<Transform, BoxCollider, Velocity>(
				[&](Entity e, Transform& t, BoxCollider& col, Velocity& v){
				glm::vec2 centerA = t.position + col.offset;
				auto candidates = m_grid.Query(centerA, col.size, e);

				glm::vec2 minA = centerA - (col.size * 0.5f);
				glm::vec2 maxA = centerA + (col.size * 0.5f);

				for(Entity entityB : candidates){
					if(e == entityB)
						continue;
					auto& transformB = reg.Get<Transform>(entityB);
					auto& colB = reg.Get<BoxCollider>(entityB);

					glm::vec2 centerB = transformB.position + colB.offset;
					glm::vec2 minB = centerB - (colB.size * 0.5f);
					glm::vec2 maxB = centerB + (colB.size * 0.5f);

					bool overlap = (minA.x < maxB.x && maxA.x > minB.x && minA.y < maxB.y && maxA.y > minB.y);
					if(overlap)
					collisions.push_back({e, entityB});
				}
				}
				);

				return collisions;
	}
	
}

