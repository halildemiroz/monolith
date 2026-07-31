#pragma once
#include <SDL.h>
#include <memory>
#include <Camera2D.h>

namespace Monolith{

	class Layer;

	class App{
		public:
			App();
			~App();

			void PushLayer(std::unique_ptr<Layer> layer);
			void Run();

		private:
			SDL_Window* m_window;
			SDL_GLContext m_glContext;
			bool m_isRunning;

			std::unique_ptr<Layer> m_layer;
			Camera2D m_cam;
	};
}
