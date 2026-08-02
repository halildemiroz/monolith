#pragma once
#include <Camera2D.h>
#include <TextureHandle.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <cstdint>

namespace Monolith{

	class Shader;
	class VAO;
	class Camera2D;

	class Renderer{
		public:
			static void Init();
			static void Clear(const glm::vec4& color);

			static void BeginScene(const Camera2D& cam);
			static void EndScene();

			static void Submit(Shader& shader, const VAO& vao, GLenum mode, GLsizei count);
			static void DrawSprite(TextureHandle texture, const glm::vec2& position, const glm::vec2& size, float rotation = 0.0f);

			static uint32_t GetDrawCallCount();
			
		private:
			static void Flush();

	};

}

