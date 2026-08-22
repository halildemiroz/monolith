#pragma once
#include <TextureHandle.h>
#include <MMath.h>

namespace Monolith{

	struct Transform{
		Vec2 position{0.0f, 0.0f};
		Vec2 prevPosition{0.0f, 0.0f};
		Vec2 scale{1.0f, 1.0f};
		float rotation = 0.0f;
	};

	struct Sprite{
		TextureHandle texture;
		Vec2 size{1.0f, 1.0f};
		Vec4 uvRect{0.0f, 0.0f, 1.0f, 1.0f};
	};

	struct Velocity{
		Vec2 value{0.0f, 0.0f};
	};

	struct BoxCollider{
		Vec2 size{1.0f, 1.0f};
		Vec2 offset{0.0f, 0.0f};
	};


}
