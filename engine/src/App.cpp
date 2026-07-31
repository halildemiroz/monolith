#include <App.h>
#include <SDL.h>
#include <Input.h>
#include <iostream>
#include <GL.h>
#include <Shader.h>
#include <VAO.h>
#include <VBO.h>
#include <memory>

namespace Monolith{
	
	App::App(){	
		if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
			std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
			m_isRunning = false;
			return;
		}
		
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		m_window = SDL_CreateWindow("Monolith Engine", 
				SDL_WINDOWPOS_CENTERED, 
				SDL_WINDOWPOS_CENTERED, 
				1280, 720, SDL_WINDOW_OPENGL);
		if(!m_window){
			std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
			m_isRunning = false;
			return;
		}
		
		m_glContext = SDL_GL_CreateContext(m_window);
		if(!m_glContext){
			std::cerr << "Failed to create GL context: " << SDL_GetError() << std::endl;
			m_isRunning = false;
			return;
		}

		if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)){
			std::cerr << "Failed to initialize GLAD" << std::endl;
			m_isRunning = false;
			return;
		}		

		float vertices[] = {
			-0.5f, -0.5f,
			0.5f, -0.5f,
			0.0f, 0.5f
		};

		m_shader = std::make_unique<Shader>(
			"#version 410 core\nlayout(location = 0) in vec2 aPos;\nvoid main(){ gl_Position = vec4(aPos, 0.0, 1.0); }",
			"#version 410 core\nout vec4 FragColor;\nvoid main(){ FragColor = vec4(1.0, 0.0, 0.0, 1.0); }"
				);

		m_vbo = std::make_unique<VBO>(vertices, sizeof(vertices));
		m_vao = std::make_unique<VAO>();
		m_vao->addVBO(*m_vbo, 0, 2, GL_FLOAT, 2 * sizeof(float), 0);

		SDL_GL_SetSwapInterval(1);

		m_isRunning = true;
	}
	
	App::~App(){
		if(m_window)
			SDL_DestroyWindow(m_window);
		if(m_glContext)
			SDL_GL_DeleteContext(m_glContext);
		SDL_Quit();
	}
	
	void App::Run(){
		Uint64 lastTime = SDL_GetPerformanceCounter();

		while(m_isRunning){
			Uint64 currentTime = SDL_GetPerformanceCounter();
			float deltaTime = (currentTime - lastTime) / (float)SDL_GetPerformanceFrequency();
			lastTime = currentTime;

			SDL_Event event;
			while(SDL_PollEvent(&event)){
				if(event.type == SDL_QUIT)
					m_isRunning = false;
			}

			Input::Update();

			if(Input::isKeyPressed(Key::Escape))
				m_isRunning = false;
			
			glClearColor(0.1f, 0.2f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			
			m_shader->Bind();
			m_vao->Bind();
			glDrawArrays(GL_TRIANGLES, 0, 3);

			SDL_GL_SwapWindow(m_window);

		}
	}

}
