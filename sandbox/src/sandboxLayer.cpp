#include <sandboxLayer.h>

#include <Shader.h>
#include <VAO.h>
#include <VBO.h>
#include <Renderer.h>
#include <glad/glad.h>
#include <memory>

sandboxLayer::sandboxLayer() = default;
sandboxLayer::~sandboxLayer() = default;

void sandboxLayer::OnAttach(){
	float vertices[] = {
		-50.0f, -50.0f,
		 50.0f, -50.0f,
		  0.0f,  50.0f
	};

	m_shader = std::make_unique<Monolith::Shader>(
			"#version 410 core\nlayout(location = 0) in vec2 aPos;\nuniform mat4 uViewProjection;\nvoid main(){ gl_Position = uViewProjection * vec4(aPos, 0.0, 1.0); }",
		"#version 410 core\nout vec4 FragColor;\nvoid main(){ FragColor = vec4(1.0, 0.0, 0.0, 1.0); }"
		);
	m_vbo = std::make_unique<Monolith::VBO>(vertices, sizeof(vertices));
	m_vao = std::make_unique<Monolith::VAO>();

	m_vao->addVBO(*m_vbo, 0, 2, GL_FLOAT, 2 * sizeof(float), 0);
}

void sandboxLayer::OnRender(){
	Monolith::Renderer::Submit(*m_shader, *m_vao, GL_TRIANGLES, 3);
}

