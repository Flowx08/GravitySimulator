#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include "Renderer.hpp"
#include <SDL2/SDL_keyboard.h>
#include "SDL2/SDL_stdinc.h"
#include <vector>
#include "particle.hpp"
#include <string>

class Simulator
{
	public:
		Simulator();
	
		void init(Renderer& r, unsigned int particlesCount);
		bool step(Renderer& r);
		void quit();

		void handleKeyDown(unsigned int key);
		void handleTextInput(char c);

		void drawUI(Renderer& r);

		float FPS;

	private:
		virtual void initializeParticles(Renderer& r, unsigned int count);
		virtual bool update(Renderer& r);
		virtual void onEnd();
		virtual void costumHandleKeyDown(unsigned int key);

	
	protected:
		//parameters
		unsigned int t;
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
		bool paused;

		//input
		unsigned int mode;
		std::string inputText;

		//UI
		bool showUI;
		
		//recording
		bool record;
		bool recordLimit;
		unsigned int recordForT;
		bool playFromRecord;
		std::string recordFilename;
		unsigned int playlenght;

		std::vector<int32_t> startPositions;
		std::vector<int32_t> pos;
		std::vector<int32_t> prediction;
		std::vector<int32_t> posDiffs;
		std::vector<int32_t> velDiffs;
		std::vector<int8_t> predictonErrors;

		//hardware
		double memoryUsage;


};

#endif
