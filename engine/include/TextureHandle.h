#pragma once
#include <cstdint>

namespace Monolith{

	struct TextureHandle{
		
		uint32_t index = 0;
		uint32_t generation = 0;

		bool isValid() const { return index != 0; }
		bool operator==(const TextureHandle& other) const {
			return index == other.index && generation == other.generation;
		}
		bool operator!=(const TextureHandle& other) const {
			return !(*this == other);
		}
	};

}
