#pragma once
#include <SDL.h>
#include <memory>
#include <mutex>

namespace Monolith{
	
	class Shader;
	class VAO;
	class VBO;

	class App{
		public:
			App();
			~App();
			
			void Run();

		private:
			SDL_Window* m_window;
			SDL_GLContext m_glContext;
			bool m_isRunning;

			std::unique_ptr<Shader> m_shader;
			std::unique_ptr<VAO> m_vao;
			std::unique_ptr<VBO> m_vbo;
	};
}
