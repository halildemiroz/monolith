#include <Texture.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>

namespace Monolith{

	Texture::Texture(const std::string& path){
		stbi_set_flip_vertically_on_load(1);

		unsigned char* data = stbi_load(path.c_str(), &m_width, &m_height, &m_channels, 0);
		if(!data){
			std::cerr << "Failed to load texture: " << path << "(" << stbi_failure_reason() << ")" << std::endl;
			return;
		}

		GLenum internalFormat = 0, dataFormat = 0;
		if(m_channels == 4){
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}else if(m_channels == 3){
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGB;
		}else{
			std::cerr << "Unsupported channel count (" << m_channels << ") for texture: " << path << std::endl;
			stbi_image_free(data);
			return;
		}

		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_2D, m_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture::~Texture(){
		if(m_id)
			glDeleteTextures(1, &m_id);
	}

	void Texture::Bind(GLuint slot) const{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_id);
	}

	void Texture::Unbind() const {
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}
