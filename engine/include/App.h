#pragma once
#include <SDL.h>
#include <SDL_video.h>

namespace Monolith{
	class App{
		public:
			App();
			~App();
			
			void Run();

		private:
			SDL_Window* m_window;
			SDL_Renderer* m_renderer;
			bool m_isRunning;
	};
}
