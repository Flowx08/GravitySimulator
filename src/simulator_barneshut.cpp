#include "simulator_barneshut.hpp"
#include "SDL2/SDL_keycode.h"
#include "SDL2/SDL_stdinc.h"
#include "bhquadtree.hpp"
#include "particle.hpp"
#include "util.hpp"
#include <algorithm>
#include <arm/types.h>
#include <sys/_types/_int32_t.h>

void SimulatorBarnesHut::initializeParticles(Renderer& r, unsigned int count)
{
	printf("Simulation: Barnes hut\n");
	
	//Get screen size
	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0, &dm);
	Uint32 windowWidth = dm.w;
	Uint32 windowHeight = dm.h;
	
	//setup simulator parameters
	drawTree = false;
	spaceTree = NULL;
	viewX = (float)windowWidth/2;
	viewY = (float)windowHeight/2;
	viewHW = -(float)windowWidth/2;
	viewHH = -(float)windowHeight/2;
	particlesCount = count;
	particles = std::vector<Particle>(particlesCount);

	//Create the tree of space
	spaceTree = new BHQuadTree((float)windowWidth/2, (float)windowHeight/2, (float)windowWidth * 2);

	//initializa particles
	float radius = 300;
	float a, d;
	for (int i = 0; i < particlesCount/2; i++) {
		particles[i].mass = 1.0f;
		a = randf() * M_PI * 2.0;
		d =  1 + randf() * (radius - 1);
		particles[i].mass = randf() * 2.0;
		particles[i].x = windowWidth + cos(a) * d; 
		particles[i].y = windowHeight / 2.0 + sin(a) * d; 
		particles[i].vx = sin(a) * 5.0 -2;
		particles[i].vy = -cos(a) * 5.0 -4;
		if (!playFromRecord) spaceTree->insert(&particles[i]);
	}
	
	for (int i = particlesCount/2; i < particlesCount; i++) {
		particles[i].mass = 1.0f;
		a = randf() * M_PI * 2.0;
		d =  1 + randf() * (radius - 1);
		particles[i].mass = randf() * 2.0;
		particles[i].x = 0 + cos(a) * d; 
		particles[i].y = windowHeight / 2.0 + sin(a) * d; 
		particles[i].vx = -sin(a) * 5.0 + 2;
		particles[i].vy = cos(a) * 5.0 + 4;
		if (!playFromRecord) spaceTree->insert(&particles[i]);
	}
}

//Draw quadtree boundaries on screen
void SimulatorBarnesHut::drawTreeBoundaries(SDL_Renderer* s, BHQuadTree* node)
{
	if (node == NULL) return;
	AABB b = node->getBoundary();
	SDL_RenderDrawLine(s, (b.cx -viewX) * viewZoom - viewHW,
			(b.cy - b.hd - viewY) * viewZoom - viewHH,
			(b.cx - viewX) * viewZoom - viewHW,
			(b.cy + b.hd -viewY) * viewZoom - viewHH);
	SDL_RenderDrawLine(s, (b.cx -b.hd -viewX) * viewZoom -viewHW,
			(b.cy - viewY) * viewZoom - viewHH,
			(b.cx + b.hd - viewX) * viewZoom - viewHW,
			(b.cy - viewY) * viewZoom - viewHH);
	drawTreeBoundaries(s, node->northWest);
	drawTreeBoundaries(s, node->northEast);
	drawTreeBoundaries(s, node->southWest);
	drawTreeBoundaries(s, node->southEast);
}

bool SimulatorBarnesHut::update(Renderer& s)
{
	// reset acceleration for each particle
	for (int i = 0; i < particlesCount; i++) {
		particles[i].ax = 0;
		particles[i].ay = 0;
	}

	//compute acceleration
	#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < particlesCount; i++)
		BHQuadTree::computeAttraction(spaceTree, &particles[i], 0.9, gForce);


	//update velocity and position
	unsigned int boundarySize = 10;
	for (int i = 0; i < particlesCount; i++) {
		particles[i].vx += particles[i].ax;
		particles[i].vy += particles[i].ay;
		particles[i].x += particles[i].vx;
		particles[i].y += particles[i].vy;
		boundarySize = fmax(boundarySize, abs(particles[i].x - viewX));
		boundarySize = fmax(boundarySize, abs(particles[i].y - viewY));

	}

	//draw particles
	if (drawParticles) {
		for (int i = 0; i < particlesCount; i++) {
			s.drawPoint((particles[i].x-viewX)*viewZoom -viewHW,
					(particles[i].y - viewY) * viewZoom - viewHH,
					1.0, 1.0, 1.0, (particles[i].mass / 2.1) * 0.6);
		}
	}

	//update quadtree
	spaceTree->clear(viewX, viewY, boundarySize);
	for (int i = 0; i < particlesCount; i++)
		spaceTree->insert(&particles[i]);
	
	//Update memory usage
	memoryUsage = (particlesCount * sizeof(Particle) + //particles buffer
			BHQuadTree::usedNodes * sizeof(BHQuadTree) + //quadtree
			startPositions.size() * sizeof(int32_t) +
			predictonErrors.size() * sizeof(int8_t) //recording buffer
			) / 1000000.f;

	//draw quadtree
	if (drawTree) {
		SDL_SetRenderDrawColor(s.getSDLRenderer(), 0x00, 0xFF, 0x00, 0xFF * 0.3);
		drawTreeBoundaries(s.getSDLRenderer(), spaceTree);
	}

	return true;
}

void SimulatorBarnesHut::onEnd()
{
}

void SimulatorBarnesHut::costumHandleKeyDown(unsigned int key)
{
	if (key == SDLK_t) {
		drawTree = !drawTree;
	}
}
