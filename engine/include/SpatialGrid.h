#pragma once
#include "Input.h"
#include <unordered_map>
#include <Entity.h>
#include <glm/glm.hpp>
#include <vector>

namespace Monolith{

	class SpatialGrid{
		public:
			SpatialGrid(float cellSize = 64.0f);
		
			void Clear();
			void Insert(Entity e, const glm::vec2& center, const glm::vec2& size);
			std::vector<Entity> Query(const glm::vec2& center, const glm::vec2& size, Entity ignoreEntity);
			
		private:
			float m_cellSize;
			std::unordered_map<uint64_t, std::vector<Entity>> m_cells;
			int32_t WorldToCell(float coord) const;
			static uint64_t CellKey(int32_t cellX, int32_t cellY);
	};
	
}
