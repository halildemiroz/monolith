#pragma once
#include "EventBus.h"
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
		explicit sandboxLayer(Monolith::Camera2D& cam, Monolith::EventBus& events);
		~sandboxLayer() override;

		void OnAttach() override;
		void OnUpdate(float deltaTime) override;
		void OnFixedUpdate(float fixedDeltaTime) override;
		void OnRender(float alpha) override;
		void OnImGuiRender() override;
	private:
		Monolith::TextureHandle m_texture;
		Monolith::TextureHandle m_playerTexture;
		Monolith::Camera2D& m_cam;
		Monolith::EventBus& m_events;
		Monolith::Registry m_registry;
		Monolith::Entity m_player;
		Monolith::SpriteSheet m_tileset;
};
