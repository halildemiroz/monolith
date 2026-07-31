#include "Camera2D.h"
#include <Renderer.h>
#include <Shader.h>
#include <VAO.h>

namespace Monolith{
	void Renderer::Init(){}

	namespace {
		glm::mat4 s_viewProjection{1.0f};
	}

	void Renderer::Clear(const glm::vec4& color){
		glClearColor(color.r, color.g, color.b, color.a);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void Renderer::BeginScene(const Camera2D& cam){
		s_viewProjection = cam.GetViewProjection();
	}

	void Renderer::EndScene(){}

	void Renderer::Submit(Shader &shader, const VAO &vao, GLenum mode, GLsizei count){
		shader.Bind();
		shader.setMat4("uViewProjection", s_viewProjection);
		vao.Bind();
		glDrawArrays(mode, 0, count);
	}
}

