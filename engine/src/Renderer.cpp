#include "Camera2D.h"
#include <Renderer.h>
#include <Shader.h>
#include <VAO.h>
#include <VBO.h>
#include <TextureLib.h>
#include <Texture.h>

#include <vector>
#include <array>
#include <memory>
#include <cmath>
#include <cstddef>

namespace Monolith{

	namespace {

		struct SpriteVertex{
			glm::vec2 position;
			glm::vec2 uv;
			float texIndex;
		};

		constexpr uint32_t MaxQuads = 10000;
		constexpr uint32_t MaxVertices = MaxQuads * 6; // no index buffer yet, 2 tris/quad
		constexpr uint32_t MaxTextureSlots = 8;

		const char* kSpriteVertexSrc =
			"#version 410 core\n"
			"layout(location = 0) in vec2 aPos;\n"
			"layout(location = 1) in vec2 aUV;\n"
			"layout(location = 2) in float aTexIndex;\n"
			"uniform mat4 uViewProjection;\n"
			"out vec2 vUV;\n"
			"out float vTexIndex;\n"
			"void main(){\n"
			"    vUV = aUV;\n"
			"    vTexIndex = aTexIndex;\n"
			"    gl_Position = uViewProjection * vec4(aPos, 0.0, 1.0);\n"
			"}\n";

		const char* kSpriteFragmentSrc =
			"#version 410 core\n"
			"in vec2 vUV;\n"
			"in float vTexIndex;\n"
			"uniform sampler2D uTextures[8];\n"
			"out vec4 FragColor;\n"
			"void main(){\n"
			"    int index = int(vTexIndex + 0.5);\n"
			"    FragColor = texture(uTextures[index], vUV);\n"
			"}\n";

		glm::mat4 s_viewProjection{1.0f};

		std::unique_ptr<VBO> s_vbo;
		std::unique_ptr<VAO> s_vao;
		std::unique_ptr<Shader> s_spriteShader;

		std::vector<SpriteVertex> s_vertexBuffer;
		std::array<TextureHandle, MaxTextureSlots> s_textureSlots{};
		uint32_t s_textureSlotCount = 0;

		uint32_t s_drawCalls = 0;
	}

	void Renderer::Init(){
		s_vertexBuffer.reserve(MaxVertices);

		s_vbo = std::make_unique<VBO>(nullptr, MaxVertices * sizeof(SpriteVertex), GL_DYNAMIC_DRAW);
		s_vao = std::make_unique<VAO>();

		s_vao->addVBO(*s_vbo, 0, 2, GL_FLOAT, sizeof(SpriteVertex), offsetof(SpriteVertex, position));
		s_vao->addVBO(*s_vbo, 1, 2, GL_FLOAT, sizeof(SpriteVertex), offsetof(SpriteVertex, uv));
		s_vao->addVBO(*s_vbo, 2, 1, GL_FLOAT, sizeof(SpriteVertex), offsetof(SpriteVertex, texIndex));

		s_spriteShader = std::make_unique<Shader>(kSpriteVertexSrc, kSpriteFragmentSrc);

		int samplers[MaxTextureSlots];
		for(uint32_t i = 0; i < MaxTextureSlots; ++i)
			samplers[i] = static_cast<int>(i);

		s_spriteShader->Bind();
		s_spriteShader->setIntArray("uTextures", samplers, MaxTextureSlots);
	}

	void Renderer::Clear(const glm::vec4& color){
		glClearColor(color.r, color.g, color.b, color.a);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void Renderer::BeginScene(const Camera2D& cam){
		s_viewProjection = cam.GetViewProjection();
		s_vertexBuffer.clear();
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

	void Renderer::DrawSprite(TextureHandle texture, const glm::vec2& position, const glm::vec2& size, float rotation){
		float texIndex = -1.0f;
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

		glm::vec2 half = size * 0.5f;
		glm::vec2 corners[4] = {
			{-half.x, -half.y}, {half.x, -half.y}, {half.x, half.y}, {-half.x, half.y}
		};

		if(rotation != 0.0f){
			float c = std::cos(rotation);
			float s = std::sin(rotation);
			for(auto& corner : corners){
				glm::vec2 rotated{ corner.x * c - corner.y * s, corner.x * s + corner.y * c };
				corner = rotated;
			}
		}

		glm::vec2 uvs[4] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};
		int order[6] = {0, 1, 2, 2, 3, 0};

		for(int idx : order){
			SpriteVertex v;
			v.position = position + corners[idx];
			v.uv = uvs[idx];
			v.texIndex = texIndex;
			s_vertexBuffer.push_back(v);
		}
	}

	void Renderer::Flush(){
		if(s_vertexBuffer.empty())
			return;

		s_vbo->setData(s_vertexBuffer.data(), s_vertexBuffer.size() * sizeof(SpriteVertex));

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

	uint32_t Renderer::GetDrawCallCount(){
		return s_drawCalls;
	}

}
