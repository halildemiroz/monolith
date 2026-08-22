#pragma once
#include <MMath.h>

namespace Monolith{

	class Camera2D{
		public:
			Camera2D(float viewportWidth, float viewportHeight);

			void setPosition(const Vec2& position);
			void setZoom(float zoom);
			void setViewportSize(float width, float height);
			
			const glm::mat4& GetViewProjection() const { return m_viewProjection; }
			const Vec2& GetPosition() const { return m_position; }
			float GetZoom() const { return m_zoom; }

		private:
			void Recalculate();

			Vec2 m_position{0.0f, 0.0f};
			float m_zoom = 1.0f;
			float m_width;
			float m_height;
			glm::mat4 m_viewProjection{1.0f};
	};

}
