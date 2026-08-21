#include "Entity.h"
#include "imgui.h"
#include <SpatialGrid.h>

namespace Monolith{

	SpatialGrid::SpatialGrid(float cellSize) : m_cellSize(cellSize){}

	void SpatialGrid::Clear(){
		m_cells.clear();
	}

	void SpatialGrid::Insert(Entity e, const glm::vec2& center, const glm::vec2& size){
		glm::vec2 min = center - (size * 0.5f);
		glm::vec2 max = center + (size * 0.5f);
		int32_t minCellX = WorldToCell(min.x);
		int32_t minCellY = WorldToCell(min.y);
		int32_t maxCellX = WorldToCell(max.x);
		int32_t maxCellY = WorldToCell(max.y);
	
		for(int32_t y = minCellY; y <= maxCellY; ++y){
			for(int32_t x = minCellX; x <= maxCellX; ++x){
				m_cells[CellKey(x,y)].push_back(e);
			}
		}	
	}

	std::vector<Entity> SpatialGrid::Query(const glm::vec2& center, const glm::vec2& size, Entity ignoreEntity){

		glm::vec2 min = center - (size * 0.5f);
		glm::vec2 max = center + (size * 0.5f);
		int32_t minCellX = WorldToCell(min.x);
		int32_t minCellY = WorldToCell(min.y);
		int32_t maxCellX = WorldToCell(max.x);
		int32_t maxCellY = WorldToCell(max.y);

		std::vector<Entity> results;
		
		for(int32_t y = minCellY; y <= maxCellY; ++y){
			for(int32_t x = minCellX; x <= maxCellX; ++x){
				auto it = m_cells.find(CellKey(x, y));
				if(it != m_cells.end()){
					for(Entity entity : it->second){
						if(entity == ignoreEntity)
							continue;

						if(std::find(results.begin(), results.end(), entity) == results.end()){
							results.push_back(entity);
						}
					}
				}
			}
		}
		return results;
	}

	int32_t SpatialGrid::WorldToCell(float coord) const {
    return static_cast<int32_t>(std::floor(coord / m_cellSize));
	}

	uint64_t SpatialGrid::CellKey(int32_t cellX, int32_t cellY) {
    uint64_t xPart = static_cast<uint64_t>(static_cast<uint32_t>(cellX)) << 32;
    uint64_t yPart = static_cast<uint32_t>(cellY);
    return xPart | yPart;
	}
	
}
