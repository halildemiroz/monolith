#pragma once
#include <Camera2D.h>
#include <TextureHandle.h>
#include <Layer.h>
#include <ECS.h>
#include <Player.h>
#include <SpriteSheet.h>

namespace Monolith{
	class Camera2D;
}

class sandboxLayer : public Monolith::Layer{
	public:
		explicit sandboxLayer(Monolith::Camera2D& cam);
		~sandboxLayer() override;

		void OnAttach() override;
		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	private:
		Monolith::TextureHandle m_texture;
		Monolith::TextureHandle m_playerTexture;
		Monolith::Camera2D& m_cam;
		Monolith::Registry m_registry;
		Monolith::Entity m_player;
		Monolith::SpriteSheet m_tileset;
};
