#include "Camera2D.h"
#include <Renderer.h>
#include <Shader.h>
#include <VAO.h>
#include <VBO.h>
#include <TextureLib.h>
#include <Texture.h>
#include <MMath.h>

#include <vector>
#include <array>
#include <memory>
#include <cmath>
#include <cstddef>

namespace Monolith{

	namespace {

		struct SpriteVertex{
			Vec2 position;
			Vec2 uv;
			Color color;
			float texIndex;
		};

		struct LineVertex{
			Vec2 position;
			Color color;
		};

		constexpr uint32_t MaxQuads = 90000;
		constexpr uint32_t MaxVertices = MaxQuads * 6;
		constexpr uint32_t MaxTextureSlots = 8;

		const char* kSpriteVertexSrc =
			"#version 410 core\n"
			"layout(location = 0) in vec2 aPos;\n"
			"layout(location = 1) in vec2 aUV;\n"
			"layout(location = 2) in vec4 aColor;\n"
			"layout(location = 3) in float aTexIndex;\n"
			"uniform mat4 uViewProjection;\n"
			"out vec2 vUV;\n"
			"out vec4 vColor;\n"
			"out float vTexIndex;\n"
			"void main(){\n"
			"    vUV = aUV;\n"
			"    vColor = aColor;\n"
			"    vTexIndex = aTexIndex;\n"
			"    gl_Position = uViewProjection * vec4(aPos, 0.0, 1.0);\n"
			"}\n";

		const char* kSpriteFragmentSrc =
			"#version 410 core\n"
			"in vec2 vUV;\n"
			"in vec4 vColor;\n"
			"in float vTexIndex;\n"
			"uniform sampler2D uTextures[8];\n"
			"out vec4 FragColor;\n"
			"void main(){\n"
			"    int index = int(vTexIndex + 0.5);\n"
			"    FragColor = texture(uTextures[index], vUV) * vColor;\n"
			"}\n";

		const char* lineVertexSrc =
			"#version 410 core\n"
			"layout(location = 0) in vec2 aPos;\n"
			"layout(location = 1) in vec4 aColor;\n"
			"uniform mat4 uViewProjection;\n"
			"out vec4 vColor;\n"
			"void main(){\n"
			"		vColor = aColor;\n"
			"		gl_Position = uViewProjection * vec4(aPos, 0.0, 1.0f);\n"
			"}\n";

		const char* lineFragmentSrc = 
			"#version 410 core\n"
			"in vec4 vColor;\n"
			"out vec4 FragColor;\n"
			"void main(){\n"
			"		FragColor = vColor;\n"
			"}\n";

		glm::mat4 s_viewProjection{1.0f};

		std::unique_ptr<VBO> s_vbo;
		std::unique_ptr<VAO> s_vao;
		std::unique_ptr<VBO> s_vbo2;
		std::unique_ptr<VAO> s_vao2;
		std::unique_ptr<Shader> s_spriteShader;
		std::unique_ptr<Shader> s_lineShader;
		std::unique_ptr<Texture> s_whiteTexture;

		std::vector<SpriteVertex> s_vertexBuffer;
		std::vector<LineVertex> s_lineVertexBuffer;
		std::array<TextureHandle, MaxTextureSlots> s_textureSlots{};
		uint32_t s_textureSlotCount = 0;

		uint32_t s_drawCalls = 0;
	}

	void Renderer::Init(){
		s_vertexBuffer.reserve(MaxVertices);
		s_lineVertexBuffer.reserve(1000);

		s_vbo = std::make_unique<VBO>(nullptr, MaxVertices * sizeof(SpriteVertex), GL_DYNAMIC_DRAW);
		s_vao = std::make_unique<VAO>();

		s_vao->addVBO(*s_vbo, 0, 2, GL_FLOAT, sizeof(SpriteVertex), offsetof(SpriteVertex, position));
		s_vao->addVBO(*s_vbo, 1, 2, GL_FLOAT, sizeof(SpriteVertex), offsetof(SpriteVertex, uv));
		s_vao->addVBO(*s_vbo, 2, 4, GL_FLOAT, sizeof(SpriteVertex), offsetof(SpriteVertex, color));
		s_vao->addVBO(*s_vbo, 3, 1, GL_FLOAT, sizeof(SpriteVertex), offsetof(SpriteVertex, texIndex));
	
		s_vbo2 = std::make_unique<VBO>(nullptr, 1000 * sizeof(LineVertex), GL_DYNAMIC_DRAW);
		s_vao2 = std::make_unique<VAO>();

		s_vao2->addVBO(*s_vbo2, 0, 2, GL_FLOAT, sizeof(LineVertex), offsetof(LineVertex, position));
		s_vao2->addVBO(*s_vbo2, 1, 4, GL_FLOAT, sizeof(LineVertex), offsetof(LineVertex, color));

		s_spriteShader = std::make_unique<Shader>(kSpriteVertexSrc, kSpriteFragmentSrc);
		s_lineShader = std::make_unique<Shader>(lineVertexSrc, lineFragmentSrc);
		s_whiteTexture = std::make_unique<Texture>(1,1,0xffffffffu);

		int samplers[MaxTextureSlots];
		for(uint32_t i = 0; i < MaxTextureSlots; ++i)
			samplers[i] = static_cast<int>(i);

		s_spriteShader->Bind();
		s_spriteShader->setIntArray("uTextures", samplers, MaxTextureSlots);
	}

	void Renderer::Clear(const Color& color){
		glClearColor(color.r, color.g, color.b, color.a);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void Renderer::BeginScene(const Camera2D& cam){
		s_viewProjection = cam.GetViewProjection();
		s_vertexBuffer.clear();
		s_lineVertexBuffer.clear();
		s_textureSlotCount = 0;
		s_drawCalls = 0;
	}

	void Renderer::EndScene(){
		Flush();
	}

	void Renderer::Submit(Shader &shader, const VAO &vao, GLenum mode, GLsizei count){
		shader.Bind();
		shader.setMat4("uViewProjection", s_viewProjection);
		vao.Bind();
		glDrawArrays(mode, 0, count);
	}

	void Renderer::DrawSprite(TextureHandle texture, const Vec2& position, const Vec2& size, float rotation, const Vec4& uvRect, const Color& tint){
		float texIndex = 0.0f;
		if(texture.index != 0 || texture.generation != 0){
			texIndex = -1.0f;
			for(uint32_t i = 0; i < s_textureSlotCount; ++i){
				if(s_textureSlots[i] == texture){
					texIndex = static_cast<float>(i);
					break;
				}
			}

			bool needsNewSlot = (texIndex < 0.0f);
			bool vertexBufferFull = s_vertexBuffer.size() + 6 > MaxVertices;
			bool textureSlotsFull = needsNewSlot && s_textureSlotCount >= MaxTextureSlots;

			if(vertexBufferFull || textureSlotsFull){
				Flush();
				needsNewSlot = true;
			}

			if(needsNewSlot){
				texIndex = static_cast<float>(s_textureSlotCount);
				s_textureSlots[s_textureSlotCount] = texture;
				s_textureSlotCount++;
			}
		} else {
			if(s_vertexBuffer.size() + 6 > MaxVertices){
				Flush();
			}
		}

		Vec2 half = size * 0.5f;
		Vec2 corners[4] = {
			{-half.x, -half.y}, {half.x, -half.y}, {half.x, half.y}, {-half.x, half.y}
		};

		if(rotation != 0.0f){
			float c = std::cos(rotation);
			float s = std::sin(rotation);
			for(auto& corner : corners){
				Vec2 rotated{ corner.x * c - corner.y * s, corner.x * s + corner.y * c };
				corner = rotated;
			}
		}
		
		Vec2 uvs[4] = {
			{uvRect.x, uvRect.y},
			{uvRect.z, uvRect.y},
			{uvRect.z, uvRect.w},
			{uvRect.x, uvRect.w}
		};

		int order[6] = {0, 1, 2, 2, 3, 0};

		for(int idx : order){
			SpriteVertex v;
			v.position = position + corners[idx];
			v.uv = uvs[idx];
			v.color = tint;
			v.texIndex = texIndex;
			s_vertexBuffer.push_back(v);
		}
	}

	void Renderer::DrawFilledRect(const Vec2& position, const Vec2& size, const Color& color, float rotation){
		DrawSprite(TextureHandle{}, position, size, rotation, Vec4(0.0f, 0.0f, 1.0f, 1.0f), color);
	}

	void Renderer::DrawTriangle(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Color& color){
		if(s_vertexBuffer.size() + 3 > MaxVertices){
			Flush();
		}
		s_vertexBuffer.push_back(SpriteVertex{p0, Vec2{0.0f, 0.0f}, color, 0.0f});
		s_vertexBuffer.push_back(SpriteVertex{p1, Vec2{0.0f, 0.0f}, color, 0.0f});
		s_vertexBuffer.push_back(SpriteVertex{p2, Vec2{0.0f, 0.0f}, color, 0.0f});
	}

	void Renderer::DrawCircle(const Vec2& center, float radius, const Color& color, int segments){
		if(segments < 3) segments = 3;
		if(s_vertexBuffer.size() + static_cast<size_t>(segments) * 3 > MaxVertices){
			Flush();
		}
		constexpr float kTwoPi = 6.28318530718f;
		float step = kTwoPi / static_cast<float>(segments);
		for(int i = 0; i < segments; ++i){
			float a0 = static_cast<float>(i) * step;
			float a1 = static_cast<float>(i + 1) * step;
			Vec2 p1 = center + Vec2{std::cos(a0) * radius, std::sin(a0) * radius};
			Vec2 p2 = center + Vec2{std::cos(a1) * radius, std::sin(a1) * radius};

			s_vertexBuffer.push_back(SpriteVertex{center, Vec2{0.0f, 0.0f}, color, 0.0f});
			s_vertexBuffer.push_back(SpriteVertex{p1, Vec2{0.0f, 0.0f}, color, 0.0f});
			s_vertexBuffer.push_back(SpriteVertex{p2, Vec2{0.0f, 0.0f}, color, 0.0f});
		}
	}

	void Renderer::Flush(){
		if(!s_vertexBuffer.empty()){
			s_vbo->setData(s_vertexBuffer.data(), s_vertexBuffer.size() * sizeof(SpriteVertex));

			for(uint32_t i = 0; i < MaxTextureSlots; ++i)
				s_whiteTexture->Bind(i);
			for(uint32_t i = 0; i < s_textureSlotCount; ++i)
				TextureLib::Get(s_textureSlots[i]).Bind(i);

			s_spriteShader->Bind();
			s_spriteShader->setMat4("uViewProjection", s_viewProjection);

			s_vao->Bind();
			glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(s_vertexBuffer.size()));

			s_drawCalls++;

			s_vertexBuffer.clear();
			s_textureSlotCount = 0;
		}

		if(!s_lineVertexBuffer.empty()){
			s_vbo2->setData(s_lineVertexBuffer.data(), s_lineVertexBuffer.size() * sizeof(LineVertex));

			s_lineShader->Bind();
			s_lineShader->setMat4("uViewProjection", s_viewProjection);

			s_vao2->Bind();
			glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(s_lineVertexBuffer.size()));

			s_lineVertexBuffer.clear();
		}
	}

	uint32_t Renderer::GetDrawCallCount(){
		return s_drawCalls;
	}

	void Renderer::DrawLine(const Vec2 &start, const Vec2 &end, const Color &color){
		s_lineVertexBuffer.push_back(LineVertex{start, color});
		s_lineVertexBuffer.push_back(LineVertex{end, color});
	}
	
	void Renderer::DrawRect(const Vec2& position, const Vec2& size, const Color& color){
		Vec2 topLeft = {position.x - size.x/2, position.y - size.y/2};
		Vec2 topRight = {position.x + size.x/2, position.y - size.y/2};
		Vec2 bottomLeft = {position.x - size.x/2, position.y + size.y/2};
		Vec2 bottomRight = {position.x + size.x/2, position.y + size.y/2};

		DrawLine(topLeft, topRight, color);
		DrawLine(topRight, bottomRight, color);
		DrawLine(bottomRight, bottomLeft, color);
		DrawLine(bottomLeft, topLeft, color);
	}
}
