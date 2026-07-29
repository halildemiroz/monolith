#pragma once
#include <glm/glm.hpp>

namespace Monolith{

	class Camera2D{
		public:
			Camera2D(float viewportWidth, float viewportHeight);

			void setPosition(const glm::vec2& position);
			void setZoom(float zoom);
			void setViewportSize(float width, float height);

			const glm::mat4& GetViewProjection() const { return m_viewProjection; }
		private:
			void Recalculate();

			glm::vec2 m_position{0.0f, 0.0f};
			float m_zoom = 1.0f;
			float m_width;
			float m_height;
			glm::mat4 m_viewProjection{1.0f};
	}

}
