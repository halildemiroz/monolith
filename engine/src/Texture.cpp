#include <Texture.h>
#include <Image.h>
#include <iostream>

namespace Monolith{

	Texture::Texture(const std::string& path){
		ImageData image = LoadImage(path);
		if(!image.isValid())
			return;

		SetData(image.pixels, image.width, image.height, image.channels);
		FreeImage(image);
	}
	
	Texture::Texture(int width, int height, uint32_t color){
		SetData(reinterpret_cast<const unsigned char*>(&color), width, height, 4);
	}

	Texture::~Texture(){
		if(m_id)
			glDeleteTextures(1, &m_id);
	}

	void Texture::SetData(const unsigned char* pixels, int width, int height, int channels){
		GLenum dataFormat = 0;
		if(channels == 4)
			dataFormat = GL_RGBA;
		else if(channels == 3)
			dataFormat = GL_RGB;
		else{
			std::cerr << "Unsupported channel count: " << channels << std::endl;
			return;
		}

		m_width = width;
		m_height = height;
		m_channels = channels;

		if(!m_id){
			glGenTextures(1, &m_id);
			glBindTexture(GL_TEXTURE_2D, m_id);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}else{
			glBindTexture(GL_TEXTURE_2D, m_id);
		}

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, pixels);

		glBindTexture(GL_TEXTURE_2D, 0);

	}

	void Texture::Bind(GLuint slot) const {
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_id);
	}

	void Texture::Unbind() const {
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}
