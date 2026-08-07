#pragma once
#include <SDL.h>
#include <memory>
#include <Camera2D.h>
#include <cstdint>

namespace Monolith{

	class Layer;

	class App{
		public:
			App();
			~App();

			void PushLayer(std::unique_ptr<Layer> layer);
			void Run();

			void SetFixedTimestep(float seconds) { m_fixedTimestep = seconds; }
			float GetFixedTimestep() const { return m_fixedTimestep; }
			uint32_t GetLastStepCount() const { return m_lastStepCount; }

			Camera2D& GetCamera() { return m_cam; }

		private:
			SDL_Window* m_window;
			SDL_GLContext m_glContext;
			bool m_isRunning;

			std::unique_ptr<Layer> m_layer;
			Camera2D m_cam;

			float m_fixedTimestep = 1.0f / 60.0f;
			float m_accumulator = 0.0f;
			uint32_t m_lastStepCount = 0;
	};
}
