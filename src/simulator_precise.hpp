#ifndef SIMULATOR_PRECISE_HPP
#define SIMULATOR_PRECISE_HPP

#include "simulator.hpp"
#include "particle.hpp"
#include <vector>
#include <string>

class SimulatorPrecise : public Simulator {
	public:
		void initializeParticles(Renderer& r, unsigned int count);
		bool update(Renderer& r);
		void onEnd();

};

#endif
