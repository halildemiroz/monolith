#include <App.h>
#include <SDL.h>
#include <Input.h>
#include <iostream>
#include <GL.h>
#include <Renderer.h>
#include <Layer.h>

namespace Monolith{
	
	App::App() : m_cam(1280.0f, 720.0f){	
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
	
		Renderer::Init();

		SDL_GL_SetSwapInterval(1);

		m_isRunning = true;
	}
	
	App::~App(){
		if(m_layer)
			m_layer->OnDetach();
		if(m_window)
			SDL_DestroyWindow(m_window);
		if(m_glContext)
			SDL_GL_DeleteContext(m_glContext);
		SDL_Quit();
	}

	void App::PushLayer(std::unique_ptr<Layer>layer){
		m_layer = std::move(layer);
		if(m_layer)
			m_layer->OnAttach();
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

			if(m_layer)
				m_layer->OnUpdate(deltaTime);
			
			Renderer::Clear(glm::vec4(0.1f, 0.2f, 0.1f, 1.0f));
			
			Renderer::BeginScene(m_cam);
			if(m_layer)
				m_layer->OnRender();
			Renderer::EndScene();

			SDL_GL_SwapWindow(m_window);

		}
	}

}
