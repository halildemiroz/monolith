#pragma once
#include <Layer.h>
#include <memory>

namespace Monolith{
	class Shader;
	class VAO;
	class VBO;
}

class sandboxLayer : public Monolith::Layer{
	public:
		sandboxLayer();
		~sandboxLayer() override;

		void OnAttach() override;
		void OnRender() override;
	private:
		std::unique_ptr<Monolith::Shader> m_shader;
		std::unique_ptr<Monolith::VAO> m_vao;
		std::unique_ptr<Monolith::VBO> m_vbo;
};
