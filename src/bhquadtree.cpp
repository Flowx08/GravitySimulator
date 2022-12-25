#include "bhquadtree.hpp"
#include <algorithm>
#include <math.h>
#include "config.hpp"
#include <stdio.h>
#include <assert.h>

unsigned int BHQuadTree::usedNodes = 0;
std::vector< std::vector<void*> > BHQuadTree::stacks(1);

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

/*
bool BHQuadTree::insert(Particle* p)
{
	assert(p != NULL);
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
	
	// We have to add the points/data contained in this quad array to the new quads if we only want
	// the last node to hold the data
	cxMass += p->x * p->mass;
	cyMass += p->y * p->mass;
	totalMass += p->mass;
	pcount++;
	assert(northWest != NULL);
	assert(northWest != NULL);
	assert(southWest != NULL);
	assert(southEast != NULL);
	if (northWest->insert(p)) return true;
	if (northEast->insert(p)) return true;
	if (southWest->insert(p)) return true;
	if (southEast->insert(p)) return true;

	// Otherwise, the point cannot be inserted for some unknown reason (this should never happen)
	return false;
}
*/

bool BHQuadTree::insert(Particle* p)
{
	std::vector<void*>& stack = stacks[0];
	stack.clear();
	stack.push_back((void*)this);
		
	while (stack.size() != 0) {
		BHQuadTree& node = *(BHQuadTree*)stack.back();
		assert(&node != NULL);

		// Ignore objects that do not belong in this quad tree
		if (!node.boundary.containsPoint(*p)) {
			stack.pop_back();
			continue;
		}

		// If there is space in this quad tree and if doesn't have subdivisions, add the object here
		if (node.pcount == 0 && node.isLeaf())
		{
			node.cxMass += p->x * p->mass;
			node.cyMass += p->y * p->mass;
			node.totalMass += p->mass;
			node.pcount++;
			node.particle = p;
			return true;
		}

		// Otherwise, subdivide and then add the point to whichever node will accept it
		if (node.isLeaf()) {
			node.northWest = new BHQuadTree(node.boundary.cx - node.boundary.hd/2.f, node.boundary.cy - node.boundary.hd/2.f, node.boundary.hd/2.f);
			node.northEast = new BHQuadTree(node.boundary.cx + node.boundary.hd/2.f, node.boundary.cy - node.boundary.hd/2.f, node.boundary.hd/2.f);
			node.southWest = new BHQuadTree(node.boundary.cx - node.boundary.hd/2.f, node.boundary.cy + node.boundary.hd/2.f, node.boundary.hd/2.f);
			node.southEast = new BHQuadTree(node.boundary.cx + node.boundary.hd/2.f, node.boundary.cy + node.boundary.hd/2.f, node.boundary.hd/2.f);
			usedNodes += 4;	
			
			//Find which childnode should hold the parent's particle 
			BHQuadTree* childNode = NULL;
			if (node.northWest->boundary.containsPoint(*node.particle)) childNode = node.northWest;
			else if (node.northEast->boundary.containsPoint(*node.particle)) childNode = node.northEast;
			else if (node.southWest->boundary.containsPoint(*node.particle)) childNode = node.southWest;
			else childNode = node.southEast;
			//else if (node.southEast->boundary.containsPoint(*node.particle)) childNode = node.southEast;
			/*
			if (childNode == NULL) {
				printf("%f %f\n", node.particle->x, node.particle->y);
				printf("%f %f %f %f\n",
						node.boundary.cx - node.boundary.hd,
						node.boundary.cy - node.boundary.hd,
						node.boundary.cx + node.boundary.hd * 2,
						node.boundary.cy + node.boundary.hd * 2);
				
				printf("%f %f %f\n",
						node.northWest->boundary.cx,
						node.northWest->boundary.cy,
						node.northWest->boundary.hd);
				
				printf("%f %f %f\n",
						node.southWest->boundary.cx,
						node.southWest->boundary.cy,
						node.southWest->boundary.hd);
				printf("%f %f %f\n",
						node.northEast->boundary.cx,
						node.northEast->boundary.cy,
						node.northEast->boundary.hd);
				printf("%f %f\n",
						node.southEast->boundary.cx - node.southEast->boundary.hd,
						node.southEast->boundary.cy - node.southEast->boundary.hd);
				printf("%d\n", node.southWest->boundary.containsPoint(*node.particle));
				printf("%d\n", node.northWest->boundary.containsPoint(*node.particle));
				printf("%d\n", node.southEast->boundary.containsPoint(*node.particle));
				printf("%d\n", node.northEast->boundary.containsPoint(*node.particle));
				printf("%d %d %d %d\n",
						node.particle->x < node.southEast->boundary.cx - node.southEast->boundary.hd,
						node.particle->x > node.southEast->boundary.cx + node.southEast->boundary.hd,
						node.particle->y < node.southEast->boundary.cy - node.southEast->boundary.hd,
						node.particle->y > node.southEast->boundary.cy + node.southEast->boundary.hd
						);
			}
			assert(childNode != NULL);
			*/
			
			//store particle in child node
			childNode->cxMass += node.particle->x * node.particle->mass;
			childNode->cyMass += node.particle->y * node.particle->mass;
			childNode->totalMass += node.particle->mass;
			childNode->pcount++;
			childNode->particle = node.particle;
		}

		// We have to add the points/data contained in this quad array to the new quads if we only want
		// the last node to hold the data
		node.cxMass += p->x * p->mass;
		node.cyMass += p->y * p->mass;
		node.totalMass += p->mass;
		node.pcount++;
		
		// Repeat the search for the child nodes
		stack.pop_back();
		stack.push_back(node.northWest);
		stack.push_back(node.northEast);
		stack.push_back(node.southWest);
		stack.push_back(node.southEast);
	}

	return false;
}

void BHQuadTree::subdivide()
{
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
	usedNodes = 0;
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

/*
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
*/

void BHQuadTree::computeAttraction(BHQuadTree* node, Particle* p,
		const float alpha, const float gForce,
		std::vector<void*>& stack)
{
	static float acc, dist, distX, distY;
	static double dist_sqr;
	
	stack.clear();
	stack.push_back((void*)node);
		
	while (stack.size() != 0) {
		assert(stack.size() < 100);
		BHQuadTree* n = (BHQuadTree*)stack.back();

		if (n == NULL || n->pcount == 0) {
			stack.pop_back();
			continue;
		}

		distX = (n->cxMass / n->totalMass - p->x);
		distY = (n->cyMass / n->totalMass - p->y);
		dist_sqr = distX * distX + distY * distY;
		dist = sqrt(dist_sqr);
		if (dist < config::minForceDistance) {
			stack.pop_back();
			continue;
		}

		if ((n->getBoundary().hd * 2 / dist < alpha) || n->isLeaf())
		{
			acc = (gForce * (n->totalMass / dist_sqr));
			p->ax += acc * (distX / dist);
			p->ay += acc * (distY / dist);
			stack.pop_back();
		}
		else
		{
			stack.pop_back();
			stack.push_back(n->northEast);
			stack.push_back(n->northWest);
			stack.push_back(n->southWest);
			stack.push_back(n->southEast);
		}
	}
}
