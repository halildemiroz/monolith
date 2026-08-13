#include "SDL_video.h"
#include <App.h>
#include <SDL.h>
#include <Input.h>
#include <iostream>
#include <GL.h>
#include <Renderer.h>
#include <Layer.h>
#include <TextureLib.h>
#include <Log.h>
#include <Events.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>

namespace Monolith{
	
	App::App() : m_cam(1280.0f, 720.0f){	

		Log::Init("monolith.log");
		MONO_INFO("Monolith starting up!");

		if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
			MONO_ERROR("Failed to initialize SDL: ", SDL_GetError());
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
				1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
		if(!m_window){
			MONO_ERROR("Failed to create window: ", SDL_GetError());
			m_isRunning = false;
			return;
		}
		
		m_glContext = SDL_GL_CreateContext(m_window);
		if(!m_glContext){
			MONO_ERROR("Failed to create GL context: ", SDL_GetError());
			m_isRunning = false;
			return;
		}

		if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)){
			MONO_ERROR("Failed to initialize GLAD");
			m_isRunning = false;
			return;
		}		
	
		Renderer::Init();
		TextureLib::Init();
		
		SDL_GL_SetSwapInterval(1);

		/* IMGUI */
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplSDL2_InitForOpenGL(m_window, m_glContext);
		ImGui_ImplOpenGL3_Init("#version 410");
		/* ------------ */

		m_isRunning = true;
	}
	
	App::~App(){
		if(m_layer)
			m_layer->OnDetach();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();

		if(m_window){
			SDL_DestroyWindow(m_window);
			TextureLib::Shutdown();
		}
		if(m_glContext)
			SDL_GL_DeleteContext(m_glContext);
		MONO_INFO("Monolith shutting down");
		SDL_Quit();
	}

	void App::PushLayer(std::unique_ptr<Layer>layer){
		m_layer = std::move(layer);
		if(m_layer)
			m_layer->OnAttach();
	}
	
	void App::Run(){
		Uint64 lastTime = SDL_GetPerformanceCounter();
		constexpr float kMaxFrameTime = 0.25f;

		while(m_isRunning){
			Uint64 currentTime = SDL_GetPerformanceCounter();
			float frameTime = (currentTime - lastTime) / (float)SDL_GetPerformanceFrequency();
			lastTime = currentTime;

			if(frameTime > kMaxFrameTime)
				frameTime = kMaxFrameTime;

			SDL_Event event;
			while(SDL_PollEvent(&event)){
				ImGui_ImplSDL2_ProcessEvent(&event);
				if(event.type == SDL_QUIT){
					m_events.Queue(WindowCloseEvent{});
					m_isRunning = false;
				}

				if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
					int width = event.window.data1;
					int height = event.window.data2;

					glViewport(0,0,width,height);
					m_cam.setViewportSize(static_cast<float>(width), static_cast<float>(height));

					m_events.Queue(WindowResizeEvent{width,height});
				}
			}
			
			Input::Update();

			m_events.DispatchQueued();

			if(Input::isKeyPressed(Key::Escape))
				m_isRunning = false;

			TextureLib::Update(frameTime);

			if(m_layer)
				m_layer->OnUpdate(frameTime);

			m_accumulator += frameTime;
			m_lastStepCount = 0;
			while(m_accumulator >= m_fixedTimestep){
				if(m_layer)
					m_layer->OnFixedUpdate(m_fixedTimestep);
				m_accumulator -= m_fixedTimestep;
				m_lastStepCount++;
			}

			float alpha = m_accumulator / m_fixedTimestep;
			
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame();
			ImGui::NewFrame();

			if(m_layer)
				m_layer->OnImGuiRender();

			ImGui::Render();
			
			Renderer::Clear(glm::vec4(0.1f, 0.2f, 0.1f, 1.0f));
			
			Renderer::BeginScene(m_cam);
			if(m_layer)
				m_layer->OnRender(alpha);
			Renderer::EndScene();
			
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


			SDL_GL_SwapWindow(m_window);

		}
	}

}
