#include <VBO.h>

namespace Monolith{

	VBO::VBO(const void* data, size_t size, GLenum usage){
		glGenBuffers(1, &m_id);
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
		glBufferData(GL_ARRAY_BUFFER, size, data, usage);
	}

	VBO::~VBO(){
		glDeleteBuffers(1, &m_id);
	}

	void VBO::Bind() const{
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
	}

	void VBO::Unbind() const{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void VBO::setData(const void* data, size_t size){
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

}
		
