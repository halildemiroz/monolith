#include "Entity.h"
#include <Systems.h>
#include <Registry.h>
#include <Components.h>
#include <Renderer.h>

namespace Monolith{

	void MovementSystem(Registry &registry, float deltaTime){
		registry.Each<Transform, Velocity>(
				[deltaTime](Entity, Transform& transform, Velocity& velocity){
					transform.prevPosition = transform.position;
					transform.position += velocity.value * deltaTime;
				});
	}

	void RenderSystem(Registry &registry, float alpha){
		registry.Each<Transform, Sprite>(
				[alpha](Entity, Transform& transform, Sprite& sprite){
					Renderer::DrawSprite(sprite.texture,
							Lerp(transform.prevPosition, transform.position, alpha),
							sprite.size * transform.scale,
							transform.rotation,
							sprite.uvRect);
				});
	}

}
