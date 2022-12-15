#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_keyboard.h>
#include "SDL_FontCache.h"
#include <vector>
#include "particle.hpp"
#include <string>

class Simulator
{
	public:
		Simulator();
		virtual void initializeParticles(SDL_Renderer* r, unsigned int count);
		virtual bool update(SDL_Renderer* r, unsigned int t);
		virtual void onEnd(unsigned int t);
		virtual void handleKeyDown(unsigned int key);

		void drawUI(SDL_Renderer* r, unsigned int t);

		float FPS;
	
	protected:
		//parameters
		Uint32 particlesCount;
		float gForce;
		float viewX;
		float viewY;
		float viewZoom;
		float viewHW;
		float viewHH;
		bool drawParticles;
		float movementSpeed;
		std::vector<Particle> particles;

		//UI
		FC_Font* font;
		bool showUI;
		
		//recording
		bool record;
		bool recordLimit;
		unsigned int recordForT;
		bool playFromRecord;
		std::string recordFilename;
		unsigned int playlenght;
		std::vector<short> positions;

		//hardware
		double memoryUsage;


};

#endif
