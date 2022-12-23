#ifndef AABB_HPP
#define AABB_HPP

#include "particle.hpp"

//Axis aligned bounding box with center position and half dimension
struct AABB
{
	double cx, cy; 
	double hd;
	
	AABB();
	AABB(double cx, double cy, double hd);

	//check if the particle is inside the AABB
	bool containsPoint(Particle& p) const;
};

#endif
