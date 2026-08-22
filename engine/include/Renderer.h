#pragma once
#include <Camera2D.h>
#include <TextureHandle.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <cstdint>
#include <MMath.h>

namespace Monolith{

	class Shader;
	class VAO;
	class Camera2D;

	class Renderer{
		public:
			static void Init();
			static void Clear(const Color& color);

			static void BeginScene(const Camera2D& cam);
			static void EndScene();

			static void Submit(Shader& shader, const VAO& vao, GLenum mode, GLsizei count);
			static void DrawSprite(TextureHandle texture, 
					const Vec2& position, 
					const Vec2& size, float rotation = 0.0f,
					const Vec4& uvRect = Vec4(0.0f, 0.0f, 1.0f, 1.0f),
					const Color& tint = Color::White);

			static void DrawFilledRect(const Vec2& position, const Vec2& size, const Color& color = Color::White, float rotation = 0.0f);
			static void DrawTriangle(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Color& color = Color::White);
			static void DrawCircle(const Vec2& center, float radius, const Color& color = Color::White, int segments = 32);

			static uint32_t GetDrawCallCount();
			static void DrawLine(const Vec2& start, const Vec2& end, const Color& color);
			static void DrawRect(const Vec2& position, const Vec2& size, const Color& color);
			
		private:
			static void Flush();

	};

}

