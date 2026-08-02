#pragma once
#include <cstdint>

namespace Monolith{

	struct TextureHandle{
		uint32_t id = 0;

		bool isValid() const { return id != 0; }
		bool operator==(const TextureHandle& other) const { return id == other.id; }
		bool operator!=(const TextureHandle& other) const { return id != other.id; }
	};

}
