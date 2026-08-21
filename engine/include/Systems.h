#pragma once

namespace Monolith {

	class Registry;

	void MovementSystem(Registry& registry, float deltaTime);
	void RenderSystem(Registry& registry, float alpha);

}
