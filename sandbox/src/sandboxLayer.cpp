#include "Camera2D.h"
#include "Components.h"
#include "Entity.h"
#include "Player.h"
#include "Systems.h"
#include "TextureHandle.h"
#include <sandboxLayer.h>

#include <cmath>

#include <Renderer.h>
#include <TextureLib.h>
#include <Texture.h>
#include <imgui.h>

sandboxLayer::sandboxLayer(Monolith::Camera2D& cam) : m_cam(cam){}
sandboxLayer::~sandboxLayer() = default;

void sandboxLayer::OnAttach(){
	m_texture = Monolith::TextureLib::Load("sandbox/assets/a.png");
	m_playerTexture = Monolith::TextureLib::Load("sandbox/assets/b.png");
	Monolith::TextureLib::LoadAsync("sandbox/assets/a.png");
	Monolith::TextureLib::LoadAsync("sandbox/assets/a.png");

	const int grid = 100;
	
	m_tileset = Monolith::SpriteSheet(
			Monolith::TextureLib::LoadAsync("sandbox/assets/tileset.png"), 16, 16
			);

	for(int y = 0; y < grid; ++y){
		for(int x = 0; x < grid; ++x){
			Monolith::Entity tile = m_registry.Create();

			Monolith::Transform transform;
			transform.position = { x * 16.0f, y * 16.0f };
			m_registry.Add<Monolith::Transform>(tile, transform);

			Monolith::Sprite sprite;
			sprite.texture = m_tileset.GetTexture();
			sprite.size = { 16.0f, 16.0f };
			sprite.uvRect = m_tileset.GetUV(0);
			m_registry.Add<Monolith::Sprite>(tile, sprite);
		}
	}

	// for(int y = 0; y < grid; ++y){
	// 	for(int x = 0; x < grid; ++x){
	// 		Monolith::Entity entity = m_registry.Create();
	//
	// 		Monolith::Transform transform;
	// 		transform.position = { (x - grid / 2) * spacing, (y - grid / 2) * spacing };
	// 		m_registry.Add<Monolith::Transform>(entity, transform);
	//
	// 		Monolith::Sprite sprite;
	// 		sprite.texture = m_texture;
	// 		sprite.size = { 16.0f, 16.0f };
	// 		m_registry.Add<Monolith::Sprite>(entity, sprite);
	//
	// 		if((x+y) % 2 == 0){
	// 			Monolith::Velocity velocity;
	// 			velocity.value = { std::sin(float(x)) * 20.0f, std::cos(float(y)) * 20.0f };
	// 			m_registry.Add<Monolith::Velocity>(entity, velocity);
	// 		}
	// 	}
	// }

	m_player = m_registry.Create();

	Monolith::Transform playerTransform;
	playerTransform.position = { 0.0f, 0.0f };
	m_registry.Add<Monolith::Transform>(m_player, playerTransform);

	Monolith::Sprite playerSprite;
	playerSprite.texture = m_playerTexture;
	playerSprite.size = { 64.0f, 64.0f };
	m_registry.Add<Monolith::Sprite>(m_player, playerSprite);

	m_registry.Add<Monolith::Velocity>(m_player);
	m_registry.Add<Player>(m_player, Player{300.0f});
}

void sandboxLayer::OnUpdate(float deltaTime){
	m_cam.setPosition(m_registry.Get<Monolith::Transform>(m_player).position);
}

void sandboxLayer::OnFixedUpdate(float fixedDeltaTime){
	PlayerSystem(m_registry);
	Monolith::MovementSystem(m_registry, fixedDeltaTime);
	m_registry.Flush();
}

void sandboxLayer::OnRender(float alpha){
	Monolith::RenderSystem(m_registry);
}

void sandboxLayer::OnImGuiRender(){
	ImGui::Begin("Camera");

	glm::vec2 position = m_cam.GetPosition();
	if(ImGui::DragFloat2("Position", &position.x, 1.0f))
		m_cam.setPosition(position);

	float zoom = m_cam.GetZoom();
	if(ImGui::DragFloat("Zoom", &zoom ,0.01f, 0.1f, 10.0f))
		m_cam.setZoom(zoom);

	ImGui::Text("Draw calls: %u", Monolith::Renderer::GetDrawCallCount());
	ImGui::Text("Frame time: %.3f ms (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	bool hotReload = Monolith::TextureLib::IsHotReloadEnabled();
	if(ImGui::Checkbox("Hot Reload", &hotReload))
		Monolith::TextureLib::SetHotReloadEnabled(hotReload);
	
	const auto& stats = Monolith::TextureLib::GetStats();
	ImGui::Separator();
	ImGui::Text("Load requests: %u", stats.loadRequests);
	ImGui::Text("Cache hits:    %u", stats.cacheHits);
	ImGui::Text("GPU uploads:   %u", stats.gpuUploads);
	ImGui::Text("Hot reloads:   %u", stats.hotReloads);
	ImGui::Text("Pending async: %u", stats.pendingAsync);
	ImGui::Text("Entities: %zu", m_registry.AliveCount());

	ImGui::End();
}
