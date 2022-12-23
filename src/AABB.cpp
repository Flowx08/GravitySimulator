#include "AABB.hpp"


AABB::AABB()
{
	cx = cy = hd = 0;
}

AABB::AABB(double cx, double cy, double hd)
{
	this->cx = cx;
	this->cy = cy;
	this->hd = hd;
}

bool AABB::containsPoint(Particle& p) const
{
	if ((double)p.x < cx - hd || (double)p.x > cx + hd) return false;
	if ((double)p.y < cy - hd || (double)p.y > cy + hd) return false;
	return true;
}
