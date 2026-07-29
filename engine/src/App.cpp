#include "SDL_keycode.h"
#include "SDL_stdinc.h"
#include "SDL_timer.h"
#include <App.h>
#include <SDL.h>
#include <SDL_events.h>
#include <iostream>

namespace Monolith{
	
	App::App(){	
		if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
			std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
			m_isRunning = false;
			return;
		}

		m_window = SDL_CreateWindow("Monolith Engine", 
				SDL_WINDOWPOS_CENTERED, 
				SDL_WINDOWPOS_CENTERED, 
				1280, 720, SDL_WINDOW_SHOWN);
		if(!m_window){
			std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
			m_isRunning = false;
			return;
		}

		m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_SOFTWARE);
		if(!m_renderer){
			std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
			return;
		}
	
		SDL_SetRenderDrawColor(m_renderer, 255,255,255,255);

		m_isRunning = true;
	}
	
	App::~App(){
		if(m_window)
			SDL_DestroyWindow(m_window);
		SDL_Quit();
	}
	
	void App::Run(){
		SDL_Event event;
		Uint64 lastTime = SDL_GetPerformanceCounter();

		while(m_isRunning){
			Uint64 currentTime = SDL_GetPerformanceCounter();
			float deltaTime = (currentTime - lastTime) / (float)SDL_GetPerformanceFrequency();
			lastTime = currentTime;

			while(SDL_PollEvent(&event)){
				if(event.type == SDL_QUIT)
					m_isRunning = false;
				if(event.type == SDL_KEYDOWN){
					if(event.key.keysym.sym == SDLK_ESCAPE)
						m_isRunning = false;
				}
			}

			SDL_Delay(16);
		}
	}


}
