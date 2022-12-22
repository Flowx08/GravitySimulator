#include <SDL2/SDL.h>
#include <cmath>
#include <cstdio>
#include <stdio.h>
#include <numeric>
#include <sys/syslimits.h>
#include <vector>
#include <iostream>
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_stdinc.h"
#include <SDL2_ttf/SDL_ttf.h>
#include "SDL2/SDL_video.h"
#include "particle.hpp"
#include "simulator_precise.hpp"
#include "simulator_barneshut.hpp"
#include "util.hpp"
#include "config.hpp"
#include <omp.h>
#include <OpenGL/gl3.h>
#include "Renderer.hpp"



int main(int argc, char* argv[]) {
	
	srand((unsigned int)NULL);

	Simulator* simulator = NULL;
	SDL_Window* window = NULL;

	SDL_Init(SDL_INIT_EVERYTHING);              // Initialize SDL2
	TTF_Init();

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	omp_set_num_threads(config::cpuCores);
	printf("CPU cores: %d\n", config::cpuCores);

	//Get screen size
	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0, &dm);
	Uint32 windowWidth = dm.w;
	Uint32 windowHeight = dm.h;
	printf("Screen size: (%d x %d)\n", windowWidth, windowHeight);

	// Create an application window with the following settings:
	window = SDL_CreateWindow(
			"Particle simulator",                  // window title
			200,           // initial x position
			128,           // initial y position
			windowWidth,                               // width, in pixels
			windowHeight,                               // height, in pixels
			SDL_WINDOW_OPENGL //| SDL_WINDOW_BORDERLESS // flags - see below
			| SDL_WINDOW_FULLSCREEN
			);


	// Check that the window was successfully created
	if (window == NULL) {
		// In the case that the window could not be made...
		printf("Could not create window: %s\n", SDL_GetError());
		return -1;
	}
	

	//Create renderer
	Renderer r;
	r.init(window);

	//create simulator
	if (std::string(config::mode) == "precise") simulator = (Simulator*)new SimulatorPrecise();
	else simulator = (Simulator*)new SimulatorBarnesHut();
	simulator->init(r, config::particlesCount);
	
	SDL_Event e;
	Uint64 start, end;
	bool running = true;
	while (running) {

		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_KEYDOWN:
					if (e.key.keysym.sym == SDLK_q)
						running = false;
					simulator->handleKeyDown(e.key.keysym.sym);
					break;
				case SDL_QUIT:
					running = false;
					break;
				case SDL_TEXTINPUT: // this handles text input events
					for(int i=0; i<SDL_TEXTINPUTEVENT_TEXT_SIZE; ++i)
					{
						char c = e.text.text[i];
						
						// cancel if a non-ascii char is encountered
						if(c < ' ' || c > '~')
							break;

						simulator->handleTextInput(c);			
					}	

					break;
				default:
					break;
			}
		}

		r.clear();

		//Update and draw simulation
		running = simulator->step(r) && running;
		
		//Draw UI
		simulator->drawUI(r);

		// And now we present everything we draw after the clear.
		r.present();

		//compute elapsed time in ms
		end = SDL_GetPerformanceCounter();
		float elapsedMs = (end - start) / (float)SDL_GetPerformanceFrequency();

		// Cap to 60 FPS
		SDL_Delay(floor(16.66f - elapsedMs * 1000.0));

		end = SDL_GetPerformanceCounter();
		elapsedMs = (end - start) / (float)SDL_GetPerformanceFrequency();
		simulator->FPS = 1.f / elapsedMs;

		start = SDL_GetPerformanceCounter();
	}

	simulator->quit();

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
