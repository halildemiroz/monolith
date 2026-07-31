#pragma once
#include <glad/glad.h>
#include <VBO.h>

namespace Monolith{

	class VAO{
		public:
			VAO();
			~VAO();

			void Bind() const;
			void Unbind() const;

			void addVBO(const VBO& vbo, GLuint index, GLint componentCount, GLenum type, GLsizei stride, size_t offset);
		private:
			GLuint m_id;
	};
}

