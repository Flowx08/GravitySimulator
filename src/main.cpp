#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <cstdio>
#include <stdio.h>
#include <numeric>
#include <vector>
#include <iostream>
#include "SDL2/SDL_stdinc.h"
#include <SDL2_ttf/SDL_ttf.h>
#include "particle.hpp"
#include "simulator_precise.hpp"
#include "simulator_barneshut.hpp"
#include "util.hpp"

int main(int argc, char* argv[]) {
	
	srand((unsigned int)NULL);

	Simulator* simulator = NULL;
	SDL_Window* window = NULL;
	SDL_Renderer* s = NULL;

	SDL_Init(SDL_INIT_EVERYTHING);              // Initialize SDL2
	TTF_Init();

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
			SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS // flags - see below
			| SDL_WINDOW_FULLSCREEN
			);

	// We create a renderer with hardware acceleration, 
	//we also present according with the vertical sync refresh.
	s = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	// Check that the window was successfully created
	if (window == NULL) {
		// In the case that the window could not be made...
		printf("Could not create window: %s\n", SDL_GetError());
		return -1;
	}

	//set blend mode
	SDL_SetRenderDrawBlendMode(s, SDL_BLENDMODE_BLEND);

	//create simulator
	//simulator = (Simulator*)new SimulatorPrecise();
	simulator = (Simulator*)new SimulatorBarnesHut();
	simulator->initializeParticles(s, 300000);

	SDL_Event e;
	Uint64 start, end;
	bool running = true;
	unsigned int t = 0;
	while (running) {

		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_KEYDOWN:
					if (e.key.keysym.sym == SDLK_q)
						running = false;
					break;
				case SDL_QUIT:
					running = false;
					break;
				default:
					simulator->handleKeyDown(e.key.keysym.sym);
					break;
			}
		}

		// We clear what we draw before
		SDL_RenderClear(s);

		// Set our color for the draw functions
		SDL_SetRenderDrawColor(s, 0xFF, 0xFF, 0xFF, 0x77);
		

		//Update and draw simulation
		running = simulator->update(s, t) && running;
		
		//Draw UI
		simulator->drawUI(s);
		
		// Set the color to what was before
		SDL_SetRenderDrawColor(s, 0x00, 0x00, 0x00, 0xFF);

		// And now we present everything we draw after the clear.
		SDL_RenderPresent(s);

		//compute elapsed time in ms
		end = SDL_GetPerformanceCounter();
		float elapsedMs = (end - start) / (float)SDL_GetPerformanceFrequency();

		// Cap to 60 FPS
		SDL_Delay(floor(0.01666f - elapsedMs));

		end = SDL_GetPerformanceCounter();
		elapsedMs = (end - start) / (float)SDL_GetPerformanceFrequency();
		printf("%f\n", elapsedMs);
		simulator->FPS = 1.f / elapsedMs;

		start = SDL_GetPerformanceCounter();

		t++;

	}

	simulator->onEnd(t);

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(s);
	SDL_Quit();

	return 0;
}