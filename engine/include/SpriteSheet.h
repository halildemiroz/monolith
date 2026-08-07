#pragma once
#include <TextureHandle.h>
#include <glm/glm.hpp>

namespace Monolith{

	class SpriteSheet{
		public:
			SpriteSheet() = default;
			SpriteSheet(TextureHandle texture, int columns, int rows) : m_texture(texture), m_columns(columns), m_rows(rows){}

			glm::vec4 GetUV(int id) const {
				if(m_columns <= 0 || m_rows <= 0 || id < 0 || id >= GetTileCount())
					return { 0.0f, 0.0f, 1.0f, 1.0f };

				int col = id % m_columns;
				int row = id / m_columns;

				float w = 1.0f / static_cast<float>(m_columns);
				float h = 1.0f / static_cast<float>(m_rows);

				float u0 = col * w;
				float v1 = 1.0f - row * h;

				return { u0, v1 - h, u0 + w, v1 };
			}

			glm::vec4 GetUV(int col, int row) const {
				return GetUV(row * m_columns + col);
			}

			TextureHandle GetTexture() const { return m_texture; }
			int GetColumns() const { return m_columns; }
			int GetRows() const { return m_rows; }
			int GetTileCount() const { return m_columns * m_rows; }

		private:
			TextureHandle m_texture;
			int m_columns = 0;
			int m_rows = 0;
	};

}
