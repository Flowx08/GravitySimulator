#include "simulator_barneshut.hpp"
#include "SDL2/SDL_keycode.h"
#include "SDL2/SDL_stdinc.h"
#include "bhquadtree.hpp"
#include "particle.hpp"
#include "util.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>

void SimulatorBarnesHut::initializeParticles(SDL_Renderer* r, unsigned int count)
{
	printf("Simulation: Barnes hut\n");
	
	//Get window size
	int windowWidth, windowHeight;
	SDL_GetRendererOutputSize(r, &windowWidth, &windowHeight);
	
	//setup simulator parameters
	drawTree = false;
	spaceTree = NULL;
	viewX = (float)windowWidth/2;
	viewY = (float)windowHeight/2;
	viewHW = -(float)windowWidth/2;
	viewHH = -(float)windowHeight/2;
	viewZoom = 0.25;
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
		spaceTree->insert(&particles[i]);
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
		spaceTree->insert(&particles[i]);
	}
	
	//Load recorded simulation results
	if (playFromRecord) {
		std::ifstream filep(recordFilename, std::ios::in | std::ifstream::binary);
		filep.read(reinterpret_cast<char *>(&playlenght), sizeof(unsigned int));
		printf("playlengh: %d\n", playlenght);
		positions = std::vector<short>(particlesCount * 2 * playlenght);
		filep.read(reinterpret_cast<char *>(&positions[0]), sizeof(short) * particlesCount * 2 * playlenght);
		printf("recording loaded!\n");
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

bool SimulatorBarnesHut::update(SDL_Renderer* s, unsigned int t)
{
	if (playFromRecord && t == playlenght) {
		return false;
	}
	if (record && recordLimit) {
		if (t >= recordForT) return false;	
	}
	if (t % 10 == 0) {
		printf("t: %d\n", t);
	}

	const Uint8* keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_D]) {
		viewX += movementSpeed;	
		movementSpeed += 1.0;
	}
	else if (keys[SDL_SCANCODE_A]) {
		viewX -= movementSpeed;
		movementSpeed += 1.0;
	}
	else if (keys[SDL_SCANCODE_W]) {
		viewY -= movementSpeed;	
		movementSpeed += 1.0;
	}
	else if (keys[SDL_SCANCODE_S]) {
		viewY += movementSpeed;	
		movementSpeed += 1.0;
	}
	else
	{
		movementSpeed = 1.0;
	}
	if (keys[SDL_SCANCODE_E])
		viewZoom *= 1.01;	
	if (keys[SDL_SCANCODE_R])
		viewZoom *= 0.99;	

	if (playFromRecord)
	{
		int px, py;
		for (int i = 0; i < particlesCount; i++)
		{
			px = (int)positions[t * particlesCount * 2 + i * 2 + 0];
			py = (int)positions[t * particlesCount * 2 + i * 2 + 1];
			SDL_SetRenderDrawColor(s, 0xFF, 0xFF, 0xFF, (particles[i].mass / 2.1) * 1.0 * 0xFF);
			SDL_RenderDrawPoint(s, (px - viewX) * viewZoom - viewHW, (py - viewY) * viewZoom - viewHH);
		}
	}
	else
	{
		// reset acceleration for each particle
		for (int i = 0; i < particlesCount; i++)
		{
			particles[i].ax = 0;
			particles[i].ay = 0;
		}
		
		//compute acceleration
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
			if (record) {
				positions.push_back((short)(particles[i].x));
				positions.push_back((short)(particles[i].y));
			}
		}

		//draw particles
		for (int i = 0; i < particlesCount; i++)
		{
			SDL_SetRenderDrawColor(s, 0xFF, 0xFF, 0xFF, (particles[i].mass / 2.1) * 0.4 * 0xFF);
			SDL_Rect r;
			r.x = (particles[i].x - viewX) * viewZoom - viewHW;
			r.y = (particles[i].y - viewY) * viewZoom - viewHH;
			r.w = fmax(1.0, viewZoom);
			r.h = fmax(1.0, viewZoom);
			SDL_RenderFillRect(s, &r);
			//SDL_RenderDrawPoint(s, (particles[i].x - viewX) * viewZoom - viewHW, (particles[i].y - viewY) * viewZoom - viewHH);
		}

		//update quadtree
		spaceTree->clear(viewX, viewY, boundarySize);
		for (int i = 0; i < particlesCount; i++)
			spaceTree->insert(&particles[i]);
		
		//draw quadtree
		if (drawTree) {
			SDL_SetRenderDrawColor(s, 0x00, 0xFF, 0x00, 0xFF * 0.3);
			drawTreeBoundaries(s, spaceTree);
		}
	}

	return true;
}

void SimulatorBarnesHut::onEnd(unsigned int t)
{
	//save positions recording
	if (record && !playFromRecord)
	{
		std::ofstream filepos("recording.bin", std::ios::out | std::ofstream::binary);
		filepos.write(reinterpret_cast<const char *>(&t), sizeof(int));
		filepos.write(reinterpret_cast<const char *>(&positions[0]), sizeof(short) * positions.size());
		printf("Saved recording!\n");
	}
}

void SimulatorBarnesHut::handleKeyDown(unsigned int key)
{
	/*
	if (key == SDLK_d)
		viewX += 0.1;	
	if (key == SDLK_a)
		viewX -= 0.1;
	if (key == SDLK_w)
		viewY -= 0.1;	
	if (key == SDLK_s)
		viewY += 0.1;	
	if (key == SDLK_e)
		viewZoom *= 1.1;	
	if (key == SDLK_r)
		viewZoom *= 0.9;	
	*/
	if (key == SDLK_t) {
		drawTree = !drawTree;
	}

}
