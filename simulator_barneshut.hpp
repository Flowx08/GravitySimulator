#ifndef SIMULATOR_BARNESHUT_HPP
#define SIMULATOR_BARNESHUT_HPP

#include "SDL2/SDL_keyboard.h"
#include "simulator.hpp"
#include "particle.hpp"
#include <vector>
#include <string>
#include "bhquadtree.hpp"

class SimulatorBarnesHut : public Simulator {
	public:
		void initializeParticles(SDL_Renderer* r, unsigned int count);
		bool update(SDL_Renderer* r, unsigned int t);
		void onEnd(unsigned int t);
		void handleKeyDown(unsigned int key);
	
	private:
		void drawTreeBoundaries(SDL_Renderer* s, BHQuadTree* node);
		
		bool drawTree = false;
		const float gForce = 0.01 * 80;
		BHQuadTree* spaceTree = NULL;
};

#endif
