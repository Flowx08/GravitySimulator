#ifndef PARTICLE_HPP
#define PARTICLE_HPP

typedef struct Particle
{
	//position
	float x;
	float y;
	
	//velocity
	float vx;
	float vy;

	//acceleration
	float ax;
	float ay;
	
	float radius;
	float mass;
} Particle;

#endif
