#ifndef BH_QUADTREE_HPP
#define BH_QUADTREE_HPP

#include <vector>
#include <memory.h>
#include "particle.hpp"
#include "AABB.hpp"

class BHQuadTree
{
	public:
		const static unsigned int NODE_CAPACITY = 64;

		BHQuadTree(float cx, float cy, float hd); 

		//insert particle in the correct leaf
		bool insert(Particle* p);

		//allocate 4 new nodes and move all the particles
		//inside the child nodes
		void subdivide();
		
		//deallocate all nodes except root
		void clear();
		void clear(float cx, float cy, float hd);
		
		//check if the node is leaf or not
		bool isLeaf();
		
		//get boundary 
		const AABB getBoundary();
		
		//Given root node of the quadtree and pointer to a particle, it computes the
		//acceleration of the particles. The parameter alpha controls the accuracy of
		//the results. A value of alpha = 0 has perfect accuracy but N^2 complexity.
		//Increasing alpha makes the computation faster but more approssimative.
		static void computeAttraction(BHQuadTree* node, Particle* p, const float alpha, const float gForce);

		// Childrens
		BHQuadTree* northWest;
		BHQuadTree* northEast;
		BHQuadTree* southWest;
		BHQuadTree* southEast;
		
		double cxMass, cyMass, totalMass;
		unsigned int pcount;
		Particle* particle;

	private:
		//deallocate all nodes except root
		static void __free(BHQuadTree* node);

		AABB boundary;
};

#endif
