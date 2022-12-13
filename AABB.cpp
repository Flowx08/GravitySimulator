#include "AABB.hpp"


AABB::AABB()
{
	cx = cy = hd = 0;
}

AABB::AABB(float cx, float cy, float hd)
{
	this->cx = cx;
	this->cy = cy;
	this->hd = hd;
}

bool AABB::containsPoint(Particle& p) const
{
	if (p.x < cx - hd || p.x > cx + hd) return false;
	if (p.y < cy - hd || p.y > cy + hd) return false;
	return true;
}
