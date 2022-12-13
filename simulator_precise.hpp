#ifndef SIMULATOR_PRECISE_HPP
#define SIMULATOR_PRECISE_HPP

#include "simulator.hpp"
#include "particle.hpp"
#include <vector>
#include <string>

class SimulatorPrecise : public Simulator {
	public:
		void initializeParticles(SDL_Renderer* r, unsigned int count);
		bool update(SDL_Renderer* r, unsigned int t);
		void onEnd(unsigned int t);
	
	private:
		const float gForce = 0.01 * 40;

};

#endif
