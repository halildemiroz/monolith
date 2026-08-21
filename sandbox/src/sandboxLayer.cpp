#include "Camera2D.h"
#include "Components.h"
#include "Entity.h"
#include "EventBus.h"
#include "Player.h"
#include "SparseSet.h"
#include "Systems.h"
#include "TextureHandle.h"
#include <sandboxLayer.h>
#include <Events.h>
#include <Log.h>
#include <cmath>
#include <CollisionSystem.h>

#include <Renderer.h>
#include <TextureLib.h>
#include <Texture.h>
#include <imgui.h>

sandboxLayer::sandboxLayer(Monolith::Camera2D& cam, Monolith::EventBus& events) : m_cam(cam), m_events(events){}
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
	
	Monolith::Entity tile = m_registry.Create();
	Monolith::Transform transform;
	transform.position = {50,50};
	transform.prevPosition = transform.position;
	m_registry.Add<Monolith::Transform>(tile, transform);

	Monolith::Sprite sprite;
	sprite.texture = m_tileset.GetTexture();
	sprite.size = {64.0f, 64.0f};
	sprite.uvRect = m_tileset.GetUV(0);
	m_registry.Add<Monolith::Sprite>(tile, sprite);

	m_registry.Add<Monolith::BoxCollider>(tile, Monolith::BoxCollider{
			.size = {64.0f, 64.0f},
			.offset = {0.0f, 0.0f}
			});

	m_player = m_registry.Create();
	Monolith::Transform playerTransform;
	playerTransform.prevPosition = { 0.0f, 0.0f };
	playerTransform.position = { 0.0f, 0.0f };
	m_registry.Add<Monolith::Transform>(m_player, playerTransform);

	Monolith::Sprite playerSprite;
	playerSprite.texture = m_playerTexture;
	playerSprite.size = { 64.0f, 64.0f };
	m_registry.Add<Monolith::Sprite>(m_player, playerSprite);

	m_registry.Add<Monolith::Velocity>(m_player);
	m_registry.Add<Player>(m_player, Player{300.0f});
	m_registry.Add<Monolith::BoxCollider>(m_player, Monolith::BoxCollider{
			.size = {64.0f, 64.0f},
			.offset = {0.0f, 0.0f}
			});

	
	m_events.Subscribe<Monolith::WindowResizeEvent>(
			[](const Monolith::WindowResizeEvent& e){
			APP_INFO("Window resized: ", e.width, "x", e.height);
			}
			);
}

void sandboxLayer::OnUpdate(float deltaTime){
}

void sandboxLayer::OnFixedUpdate(float fixedDeltaTime){
	PlayerSystem(m_registry);
	Monolith::MovementSystem(m_registry, fixedDeltaTime);

	auto collisions = m_colsys.Update(m_registry);
	for(const auto& pair : collisions)
		APP_INFO("Collision detected between entity ", pair.a.index, " and ", pair.b.index);

	m_registry.Flush();
}

void sandboxLayer::OnRender(float alpha){
	auto& t = m_registry.Get<Monolith::Transform>(m_player);
	glm::vec2 interpolated = glm::mix(t.prevPosition, t.position, alpha);
	m_cam.setPosition(interpolated);

	Monolith::RenderSystem(m_registry, alpha);

	m_registry.Each<Monolith::Transform, Monolith::BoxCollider>(
			[alpha](Monolith::Entity entity, Monolith::Transform& t, Monolith::BoxCollider& col){
			glm::vec2 pos = glm::mix(t.prevPosition, t.position, alpha);
			glm::vec2 rectMin = pos + col.offset * (col.size * 0.5f);
			Monolith::Renderer::DrawRect(rectMin, col.size, {0.0f, 1.0f, 0.0f, 1.0f});
			}
			);
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
