#ifndef RAYTRACER_NODE_H
#define RAYTRACER_NODE_H

#include <DirectXMath.h>
#include <vector>

#include "Triangle.h"

#define NUM_CHILDREN 8
#define MAX_NUM_TRIS 10

struct Node
{
	unsigned int numTris;
	int triIndices[MAX_NUM_TRIS];
	int children[NUM_CHILDREN];

	Node();
	Node(std::vector<unsigned int> p_triIndices, std::vector<unsigned int> p_children);
};

class LinkedNode
{
public:
	LinkedNode();
	LinkedNode(DirectX::XMFLOAT3 p_min, DirectX::XMFLOAT3 p_max);
	~LinkedNode();

	void setMin(DirectX::XMFLOAT3 p_min);
	void setMax(DirectX::XMFLOAT3 p_max);

	void addChildIndex(unsigned int p_index);

	LinkedNode* getChild(unsigned int p_index);

	std::vector<unsigned int> getTriIndices();
	std::vector<unsigned int> getChildIndices();

	void subdivide(DirectX::XMFLOAT3 p_min,
		DirectX::XMFLOAT3 p_max,
		std::vector<unsigned int> p_indices,
		std::vector<Triangle> p_triangles);
private:
	bool triangleIntersect(Triangle p_tri);

	DirectX::XMFLOAT3 m_min;
	DirectX::XMFLOAT3 m_max;

	std::vector<unsigned int> m_triIndices;
	std::vector<unsigned int> m_childIndices;

	LinkedNode* m_children[NUM_CHILDREN];
};

#endif //RAYTRACER_NODE_H