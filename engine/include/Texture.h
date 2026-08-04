#pragma once

#include <glad/glad.h>
#include <string>

namespace Monolith{

	class Texture{
		public:
			explicit Texture(const std::string& path);
			Texture(int width, int height, uint32_t color = 0xffffffff);
			~Texture();

			Texture(const Texture&) = delete;
			Texture& operator=(const Texture&) = delete;
			
			void SetData(const unsigned char* pixels, int width, int height, int channels);

			void Bind(GLuint slot = 0) const;
			void Unbind() const;

			int GetWidth() const { return m_width; }
			int GetHeight() const { return m_height; }
			GLuint GetID() const { return m_id; }

		private:
			GLuint m_id = 0;
			int m_width = 0;
			int m_height = 0;
			int m_channels = 0;
	};

}
