#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Monolith{

	class Shader;
	class VAO;

	class Renderer{
		public:
			static void Init();
			static void Clear(const glm::vec4& color);
			static void Submit(Shader& shader, const VAO& vao, GLenum mode, GLsizei count);
	};

}

