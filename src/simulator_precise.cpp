#include "simulator_precise.hpp"
#include "util.hpp"
#include <fstream>
#include <sstream>
#include "config.hpp"

void SimulatorPrecise::initializeParticles(SDL_Renderer* r, unsigned int count)
{
	printf("Simulation: precise\n");

	int windowWidth, windowHeight;
	SDL_GetRendererOutputSize(r, &windowWidth, &windowHeight);
	
	//setup simulator parameters
	viewX = (float)windowWidth/2;
	viewY = (float)windowHeight/2;
	viewHW = -(float)windowWidth/2;
	viewHH = -(float)windowHeight/2;
	viewZoom = 0.25;
	particlesCount = count;
	particles = std::vector<Particle>(particlesCount);

	//initializa particles
	float radius = 300;
	float a, d;
	for (int i = 0; i < particlesCount; i++) {
		particles[i].mass = 1.0f;
		a = randf() * M_PI * 2.0;
		d =  1 + randf() * (radius - 1);
		particles[i].mass = randf() * 2.0;
		particles[i].x = windowWidth / 2.0 + cos(a) * d; 
		particles[i].y = windowHeight / 2.0 + sin(a) * d; 
		particles[i].vx = sin(a) * 2.0;
		particles[i].vy = -cos(a) * 2.0;
	}

	if (playFromRecord) {
		std::ifstream filep(recordFilename, std::ios::in | std::ifstream::binary);
		filep.read(reinterpret_cast<char *>(&playlenght), sizeof(unsigned int));
		printf("playlengh: %d\n", playlenght);
		positions = std::vector<short>(particlesCount * 2 * playlenght);
		filep.read(reinterpret_cast<char *>(&positions[0]), sizeof(short) * particlesCount * 2 * playlenght);
		printf("recording loaded!\n");
	}
}

bool SimulatorPrecise::update(SDL_Renderer* s, unsigned int t)
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

	if (playFromRecord)
	{
		int px, py;
		for (int i = 0; i < particlesCount; i++)
		{
			px = (int)positions[t * particlesCount * 2 + i * 2 + 0];
			py = (int)positions[t * particlesCount * 2 + i * 2 + 1];
			SDL_SetRenderDrawColor(s, 0xFF, 0xFF, 0xFF, (particles[i].mass / 2.1) * 0.8 * 0xFF);
			SDL_RenderDrawPoint(s, px, py);
		}
		
		memoryUsage = (particlesCount * sizeof(Particle) + //particles buffer
				positions.size() * sizeof(short) //recording buffer
				) / 1000000.f;
	}
	else
	{
		float acc, dist, distX, distY;

		// reset acceleration for each particle
		for (int i = 0; i < particlesCount; i++)
		{
			particles[i].ax = 0;
			particles[i].ax = 0;
		}

		// update particle velocity and position and draw them
		#pragma omp parallel for schedule(dynamic)
		for (int i = 0; i < particlesCount; i++)
		{
			for (int k = i + 1; k < particlesCount; k++)
			{
				distX = (particles[k].x - particles[i].x);
				distY = (particles[k].y - particles[i].y);

				double dist_sqr = distX * distX + distY * distY;
				dist = sqrt(dist_sqr);
				if (dist < config::minForceDistance)
					continue;

				acc = (gForce * particles[k].mass) / dist_sqr;
				particles[i].ax += acc * (distX / dist);
				particles[i].ay += acc * (distY / dist);
				particles[i].vx += acc * (distX / dist);
				particles[i].vy += acc * (distY / dist);

				acc = -(gForce * particles[i].mass) / dist_sqr;
				particles[k].ax += acc * (distX / dist);
				particles[k].ay += acc * (distY / dist);
				particles[k].vx += acc * (distX / dist);
				particles[k].vy += acc * (distY / dist);

			}

			//particles[i].vx += particles[i].ax;
			//particles[i].vy += particles[i].ay;
			particles[i].x += particles[i].vx;
			particles[i].y += particles[i].vy;

			if (record) {
				positions.push_back((short)(particles[i].x));
				positions.push_back((short)(particles[i].y));
			}
		}
			
		// draw particle
		if (drawParticles) {
			for (int i = 0; i < particlesCount; i++)
			{
				SDL_SetRenderDrawColor(s, 0xFF, 0xFF, 0xFF, (particles[i].mass / 2.1) * 0.8 * 0xFF);
				SDL_RenderDrawPoint(s, particles[i].x, particles[i].y);
			}
		}
		
		memoryUsage = (particlesCount * sizeof(Particle) + //particles buffer
				positions.size() * sizeof(short) //recording buffer
				) / 1000000.f;
	}

	return true;
}

void SimulatorPrecise::onEnd(unsigned int t)
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
