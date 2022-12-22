#ifndef SIMULATOR_BARNESHUT_HPP
#define SIMULATOR_BARNESHUT_HPP

#include "Renderer.hpp"
#include "SDL2/SDL_keyboard.h"
#include "simulator.hpp"
#include "particle.hpp"
#include <vector>
#include <string>
#include "bhquadtree.hpp"

class SimulatorBarnesHut : public Simulator {
	public:
		void initializeParticles(Renderer& r, unsigned int count);
		bool update(Renderer& r);
		void onEnd();
		void costumHandleKeyDown(unsigned int key);
	
	private:
		void drawTreeBoundaries(SDL_Renderer* s, BHQuadTree* node);
		
		bool drawTree;
		BHQuadTree* spaceTree;
};

#endif
