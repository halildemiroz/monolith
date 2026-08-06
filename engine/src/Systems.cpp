#include "Entity.h"
#include <Systems.h>
#include <Registry.h>
#include <Components.h>
#include <Renderer.h>

namespace Monolith{

	void MovementSystem(Registry &registry, float deltaTime){
		registry.Each<Transform, Velocity>(
				[deltaTime](Entity, Transform& transform, Velocity& velocity){
					transform.position += velocity.value * deltaTime;
				});
	}

	void RenderSystem(Registry &registry){
		registry.Each<Transform, Sprite>(
				[](Entity, Transform& transform, Sprite& sprite){
					Renderer::DrawSprite(sprite.texture,
							transform.position,
							sprite.size * transform.scale,
							transform.rotation);
				});
	}

}
