#include <Renderer.h>
#include <Shader.h>
#include <VAO.h>

namespace Monolith{
	void Renderer::Init(){

	}

	void Renderer::Clear(const glm::vec4& color){
		glClearColor(color.r, color.g, color.b, color.a);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void Renderer::Submit(Shader &shader, const VAO &vao, GLenum mode, GLsizei count){
		shader.Bind();
		vao.Bind();
		glDrawArrays(mode, 0, count);
	}
}

