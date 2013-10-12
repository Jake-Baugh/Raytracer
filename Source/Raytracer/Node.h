#ifndef RAYTRACER_NODE_H
#define RAYTRACER_NODE_H

#include <DirectXMath.h>
#include <vector>

#include "Triangle.h"

#define NUM_CHILDREN 8
#define MAX_NUM_TRIS 10

class Node
{
public:
	Node();
	Node(DirectX::XMFLOAT3 p_min, DirectX::XMFLOAT3 p_max);
	~Node();

	void setMin(DirectX::XMFLOAT3 p_min);
	void setMax(DirectX::XMFLOAT3 p_max);

	void addPrimitive(unsigned int p_primitive);

	void subdivide(DirectX::XMFLOAT3 p_min,
		DirectX::XMFLOAT3 p_max,
		std::vector<unsigned int> p_indices,
		std::vector<Triangle> p_triangles);
private:
	bool triangleIntersect(Triangle p_tri);

	DirectX::XMFLOAT3 m_min;
	DirectX::XMFLOAT3 m_max;

	std::vector<unsigned int> m_primitives;

	Node* m_children[NUM_CHILDREN];
};

#endif //RAYTRACER_NODE_H