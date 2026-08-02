#include "Camera2D.h"
#include <sandboxLayer.h>

#include <Shader.h>
#include <VAO.h>
#include <VBO.h>
#include <Renderer.h>
#include <glad/glad.h>
#include <memory>
#include <imgui.h>
#include <TextureLib.h>
#include <Texture.h>

sandboxLayer::sandboxLayer(Monolith::Camera2D& cam) : m_cam(cam){}
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

	m_texture = Monolith::TextureLib::Load("sandbox/assets/a.png");

}

void sandboxLayer::OnRender(){
	Monolith::Renderer::Submit(*m_shader, *m_vao, GL_TRIANGLES, 3);
}

void sandboxLayer::OnImGuiRender(){
	ImGui::Begin("Camera");

	glm::vec2 position = m_cam.GetPosition();
	if(ImGui::DragFloat2("Position", &position.x, 1.0f))
		m_cam.setPosition(position);

	float zoom = m_cam.GetZoom();
	if(ImGui::DragFloat("Zoom", &zoom ,0.01f, 0.1f, 10.0f))
		m_cam.setZoom(zoom);

	auto& tex = Monolith::TextureLib::Get(m_texture);
	ImGui::Text("Texture: %dx%d (id=%u)", tex.GetWidth(), tex.GetHeight(), tex.GetID());

	ImGui::End();
}

