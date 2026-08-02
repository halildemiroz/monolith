#pragma once
#include <Camera2D.h>
#include <TextureHandle.h>
#include <Layer.h>
#include <memory>

namespace Monolith{
	class Camera2D;
}

class sandboxLayer : public Monolith::Layer{
	public:
		explicit sandboxLayer(Monolith::Camera2D& cam);
		~sandboxLayer() override;

		void OnAttach() override;
		void OnRender() override;
		void OnImGuiRender() override;
	private:
		Monolith::TextureHandle m_texture;
		Monolith::Camera2D& m_cam;
};
