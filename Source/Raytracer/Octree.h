#ifndef RAYTRACER_OCTREE_H
#define RAYTRACER_OCTREE_H

#include <vector>

#include "Node.h"
#include "Triangle.h"

class Octree
{
public:
	Octree();
	~Octree();

	void init(std::vector<Triangle> p_triangles);
	
	std::vector<Node>& getNodes();

private:

	void subdivide(DirectX::XMFLOAT3 p_min, DirectX::XMFLOAT3 p_max, unsigned int p_currentNode, unsigned int p_levels);

	std::vector<Node> m_nodes;
};

#endif //RAYTRACER_OCTREE_H