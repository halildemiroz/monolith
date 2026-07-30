#pragma once
#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>

namespace Monolith{
	
	class Shader{
		public:
			Shader(const std::string& vertexSrc, const std::string &fragmentSrc);
			~Shader();

			void Bind();
			void Unbind();
			void setMat4(const std::string& name, const glm::mat4& value);
			void setVec4(const std::string& name, const glm::vec4& value);
		private:
			GLuint compileStage(GLenum stage, const std::string& src);
			GLint getUniformLocation(const std::string& name);
			GLuint m_id;
	};
}

