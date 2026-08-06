#include <Player.h>
#include <Registry.h>
#include <Components.h>
#include <Input.h>
#include <glm/glm.hpp>

void PlayerSystem(Monolith::Registry& registry){
	registry.Each<Monolith::Velocity, Player>(
		[](Monolith::Entity, Monolith::Velocity& velocity, Player& player){
			glm::vec2 direction{0.0f, 0.0f};

			if(Monolith::Input::isKeyDown(Monolith::Key::W)) direction.y += 1.0f;
			if(Monolith::Input::isKeyDown(Monolith::Key::S)) direction.y -= 1.0f;
			if(Monolith::Input::isKeyDown(Monolith::Key::A)) direction.x -= 1.0f;
			if(Monolith::Input::isKeyDown(Monolith::Key::D)) direction.x += 1.0f;

			if(direction.x != 0.0f || direction.y != 0.0f)
				direction = glm::normalize(direction);

			velocity.value = direction * player.speed;
		});
}
