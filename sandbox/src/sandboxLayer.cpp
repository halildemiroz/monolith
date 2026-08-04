#include "Camera2D.h"
#include "TextureHandle.h"
#include <sandboxLayer.h>

#include <Renderer.h>
#include <TextureLib.h>
#include <Texture.h>
#include <imgui.h>

sandboxLayer::sandboxLayer(Monolith::Camera2D& cam) : m_cam(cam){}
sandboxLayer::~sandboxLayer() = default;

void sandboxLayer::OnAttach(){
	m_texture = Monolith::TextureLib::Load("sandbox/assets/a.png");
	Monolith::TextureLib::LoadAsync("sandbox/assets/a.png");
	Monolith::TextureLib::LoadAsync("sandbox/assets/a.png");
}

void sandboxLayer::OnRender(){
	int grid = 300; // 100x100 = 10,000 sprites
	float spacing = 20.0f;

	for(int y = 0; y < grid; ++y){
		for(int x = 0; x < grid; ++x){
			glm::vec2 pos{
				(x - grid / 2) * spacing,
				(y - grid / 2) * spacing
			};
			Monolith::Renderer::DrawSprite(m_texture, pos, {16.0f, 16.0f});
		}
	}
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

	ImGui::End();
}
