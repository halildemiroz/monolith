#pragma once
#include <cstdint>

namespace Monolith{

	struct Entity{
		uint32_t index = 0;
		uint32_t generation = 0;

		bool IsNull() const { return index == 0; }
		bool operator==(const Entity& other) const {
			return index == other.index && generation == other.generation;
		}
		bool operator!=(const Entity& other) const { return !(*this == other); }
	};

	inline constexpr Entity nullEntity{};

}
