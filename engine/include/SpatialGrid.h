#pragma once
#include <unordered_map>
#include <Entity.h>
#include <MMath.h>
#include <vector>

namespace Monolith{

	class SpatialGrid{
		public:
			SpatialGrid(float cellSize = 64.0f);
		
			void Clear();
			void Insert(Entity e, const Vec2& center, const Vec2& size);
			std::vector<Entity> Query(const Vec2& center, const Vec2& size, Entity ignoreEntity);
			
		private:
			float m_cellSize;
			std::unordered_map<uint64_t, std::vector<Entity>> m_cells;
			int32_t WorldToCell(float coord) const;
			static uint64_t CellKey(int32_t cellX, int32_t cellY);
	};
	
}
