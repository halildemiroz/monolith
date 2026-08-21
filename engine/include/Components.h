#pragma once
#include "glm/fwd.hpp"
#include <TextureHandle.h>
#include <glm/glm.hpp>

namespace Monolith{

	struct Transform{
		glm::vec2 position{0.0f, 0.0f};
		glm::vec2 prevPosition{0.0f, 0.0f};
		glm::vec2 scale{1.0f, 1.0f};
		float rotation = 0.0f;
	};

	struct Sprite{
		TextureHandle texture;
		glm::vec2 size{1.0f, 1.0f};
		glm::vec4 uvRect{0.0f, 0.0f, 1.0f, 1.0f};
	};

	struct Velocity{
		glm::vec2 value{0.0f, 0.0f};
	};

	struct BoxCollider{
		glm::vec2 size{1.0f, 1.0f};
		glm::vec2 offset{0.0f, 0.0f};
	};


}
