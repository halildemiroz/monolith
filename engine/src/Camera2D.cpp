#include <Camera2D.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Monolith {
	
	Camera2D::Camera2D(float viewportWidth, float viewportHeight)
		: m_width(viewportWidth), m_height(viewportHeight){
			Recalculate();
		}
	
	void Camera2D::setPosition(const glm::vec2& position){
		m_position = position;
		Recalculate();
	}

	void Camera2D::setZoom(float zoom){
		m_zoom = zoom;
		Recalculate();
	}

	  void Camera2D::setViewportSize(float width, float height){
			m_width = width;
      m_height = height;
      Recalculate();
  }

		void Camera2D::Recalculate(){
      float halfW = (m_width  * 0.5f) / m_zoom;
      float halfH = (m_height * 0.5f) / m_zoom;

      glm::mat4 projection = glm::ortho(-halfW, halfW, -halfH, halfH, -1.0f, 1.0f);
      glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-m_position, 0.0f));

      m_viewProjection = projection * view;
  }


}
