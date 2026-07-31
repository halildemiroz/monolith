#pragma once
#include <glad/glad.h>
#include <cstddef>

namespace Monolith{

	class VBO{
		public:
			VBO(const void* data, size_t size, GLenum usage = GL_STATIC_DRAW);
			~VBO();

			void Bind() const;
			void Unbind() const;
			void setData(const void* data, size_t size);
		private:
			GLuint m_id;
	};

}

