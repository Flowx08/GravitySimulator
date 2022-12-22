#include "bhquadtree.hpp"
#include <algorithm>
#include <math.h>
#include "config.hpp"
#include <stdio.h>

std::vector<BHQuadTree> BHQuadTree::nodes = std::vector<BHQuadTree>(16);
unsigned int BHQuadTree::usedNodes = 0;

BHQuadTree* BHQuadTree::getNewNode(float cx, float cy, float hd) {

	//increase allocation if needed
	if (usedNodes == nodes.size()) {
		nodes.push_back(BHQuadTree());
		printf("%d\n", usedNodes);
	}
	
	//clean node for usage
	nodes[usedNodes].northEast = NULL;
	nodes[usedNodes].northWest = NULL;
	nodes[usedNodes].southWest = NULL;
	nodes[usedNodes].southEast = NULL;
	nodes[usedNodes].cxMass = 0;
	nodes[usedNodes].cyMass = 0;
	nodes[usedNodes].totalMass = 0;
	nodes[usedNodes].pcount = 0;
	nodes[usedNodes].particle = NULL;
	nodes[usedNodes].boundary.cx = cx;
	nodes[usedNodes].boundary.cy = cy;
	nodes[usedNodes].boundary.hd = hd;
	
	//assign node
	return &nodes[usedNodes++];
}

BHQuadTree::BHQuadTree()
{
	boundary.cx = 100;
	boundary.cy = 100;
	boundary.hd = 50;
	northWest = NULL;
	northEast = NULL;
	southWest = NULL;
	southEast = NULL;

	cxMass = 0;
	cyMass = 0;
	totalMass = 0;
	pcount = 0;
	particle = NULL;
	usedNodes = 0;
}

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
	usedNodes = 0;
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
		if (pcount == 0) {
			cxMass += p->x * p->mass;
			cyMass += p->y * p->mass;
			totalMass += p->mass;
			pcount++;
			particle = p;
			return true;
		}
		else if (sqrt((p->x - cxMass/totalMass) * (p->x - cxMass/totalMass)
					+ (p->y - cyMass/totalMass) * (p->y - cyMass/totalMass)) < config::minForceDistance)
		{
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
	usedNodes += 4;	
	/*
	northWest = getNewNode(boundary.cx - boundary.hd/2, boundary.cy - boundary.hd/2, boundary.hd/2);
	northEast = getNewNode(boundary.cx + boundary.hd/2, boundary.cy - boundary.hd/2, boundary.hd/2);
	southWest = getNewNode(boundary.cx - boundary.hd/2, boundary.cy + boundary.hd/2, boundary.hd/2);
	southEast = getNewNode(boundary.cx + boundary.hd/2, boundary.cy + boundary.hd/2, boundary.hd/2);
	*/
	
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
	usedNodes = 0;
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
	if (dist < config::minForceDistance) return;

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
