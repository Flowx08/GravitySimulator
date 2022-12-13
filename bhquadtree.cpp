#include "bhquadtree.hpp"
#include <algorithm>
#include <math.h>

BHQuadTree::BHQuadTree(float cx, float cy, float hd)
{
	boundary.cx = cx;
	boundary.cy = cy;
	boundary.hd = hd;
	northWest = NULL;
	northEast = NULL;
	southWest = NULL;
	southEast = NULL;

	cxMass = 0;
	cyMass = 0;
	totalMass = 0;
	pcount = 0;
	particle = NULL;
}

bool BHQuadTree::insert(Particle* p)
{
	// Ignore objects that do not belong in this quad tree
	if (!boundary.containsPoint(*p))
		return false; // object cannot be added

	// If there is space in this quad tree and if doesn't have subdivisions, add the object here
	if (pcount == 0 && isLeaf())
	{
		cxMass += p->x * p->mass;
		cyMass += p->y * p->mass;
		totalMass += p->mass;
		pcount++;
		particle = p;
		return true;
	}
	
	// Otherwise, subdivide and then add the point to whichever node will accept it
	if (isLeaf())
		subdivide();
	
	/*
	if (isLeaf())
	{
		if (pcount == 0 || (fabs(p->x - cxMass/totalMass) < 1.0 && fabs(p->y - cyMass/totalMass) < 1.0)) {
			cxMass += p->x * p->mass;
			cyMass += p->y * p->mass;
			totalMass += p->mass;
			pcount++;
			particle = p;
			return true;
		}
		else
		{
			// Otherwise, subdivide and then add the point to whichever node will accept it
			subdivide();
		}
	}
	*/

	// We have to add the points/data contained in this quad array to the new quads if we only want
	// the last node to hold the data
	cxMass += p->x * p->mass;
	cyMass += p->y * p->mass;
	totalMass += p->mass;
	pcount++;
	if (northWest->insert(p)) return true;
	if (northEast->insert(p)) return true;
	if (southWest->insert(p)) return true;
	if (southEast->insert(p)) return true;

	// Otherwise, the point cannot be inserted for some unknown reason (this should never happen)
	return false;
}

void BHQuadTree::subdivide()
{
	northWest = new BHQuadTree(boundary.cx - boundary.hd/2, boundary.cy - boundary.hd/2, boundary.hd/2);
	northEast = new BHQuadTree(boundary.cx + boundary.hd/2, boundary.cy - boundary.hd/2, boundary.hd/2);
	southWest = new BHQuadTree(boundary.cx - boundary.hd/2, boundary.cy + boundary.hd/2, boundary.hd/2);
	southEast = new BHQuadTree(boundary.cx + boundary.hd/2, boundary.cy + boundary.hd/2, boundary.hd/2);
	
	northWest->insert(particle);
	northEast->insert(particle);
	southWest->insert(particle);
	southEast->insert(particle);
	particle = NULL;
}

void BHQuadTree::__free(BHQuadTree* node)
{
	if (node == NULL) return;
	__free(node->northEast);
	__free(node->northWest);
	__free(node->southEast);
	__free(node->southWest);
	node->northEast = NULL;
	node->northWest = NULL;
	node->southWest = NULL;
	node->southEast = NULL;
	delete node;
}

void BHQuadTree::clear()
{
	__free(northEast);
	__free(northWest);
	__free(southEast);
	__free(southWest);

	northEast = NULL;
	northWest = NULL;
	southWest = NULL;
	southEast = NULL;

	cxMass = 0;
	cyMass = 0;
	totalMass = 0;
	pcount = 0;
	particle = NULL;
}

void BHQuadTree::clear(float cx, float cy, float hd)
{
	clear();
	boundary.cx = cx;
	boundary.cy = cy;
	boundary.hd = hd;
}

bool BHQuadTree::isLeaf()
{
	if (northWest == NULL) return true;
	else return false;
}

const AABB BHQuadTree::getBoundary()
{
	return boundary;
}

void BHQuadTree::computeAttraction(BHQuadTree* node, Particle* p, const float alpha, const float gForce)
{
	static float acc, dist, distX, distY;
	static double dist_sqr;

	if (node == NULL) return;
	if (node->pcount == 0) return;
		
	distX = (node->cxMass / node->totalMass - p->x);
	distY = (node->cyMass / node->totalMass - p->y);
	dist_sqr = distX * distX + distY * distY;
	dist = sqrt(dist_sqr);
	if (dist < 1) return;

	if ((node->getBoundary().hd * 2 / dist < alpha) || node->isLeaf())
	{
		acc = (gForce * (node->totalMass / dist_sqr));
		p->ax += acc * (distX / dist);
		p->ay += acc * (distY / dist);
	}
	else
	{
		computeAttraction(node->northEast, p, alpha, gForce);
		computeAttraction(node->northWest, p, alpha, gForce);
		computeAttraction(node->southWest, p, alpha, gForce);
		computeAttraction(node->southEast, p, alpha, gForce);
	}
}
