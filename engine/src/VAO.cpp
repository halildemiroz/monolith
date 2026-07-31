#include <VAO.h>

namespace Monolith{

	VAO::VAO(){
		glGenVertexArrays(1, &m_id);
	}

	VAO::~VAO(){
		glDeleteVertexArrays(1, &m_id);
	}

	void VAO::Bind() const{
		glBindVertexArray(m_id);
	}

	void VAO::Unbind() const{
		glBindVertexArray(0);
	}

	void VAO::addVBO(const VBO& vbo, GLuint index, GLint componentCount, GLenum type, GLsizei stride, size_t offset){
		glBindVertexArray(m_id);
		vbo.Bind();

		glVertexAttribPointer(index, componentCount, type, GL_FALSE, stride, (const void*)offset);
		glEnableVertexAttribArray(index);
	}
}
