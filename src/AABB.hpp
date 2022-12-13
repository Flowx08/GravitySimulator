#ifndef AABB_HPP
#define AABB_HPP

#include "particle.hpp"

//Axis aligned bounding box with center position and half dimension
struct AABB
{
	float cx, cy; 
	float hd;
	
	AABB();
	AABB(float cx, float cy, float hd);

	//check if the particle is inside the AABB
	bool containsPoint(Particle& p) const;
};

#endif
