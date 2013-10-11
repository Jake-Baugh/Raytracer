#ifndef RAYTRACER_NODE_H
#define RAYTRACER_NODE_H

#include <DirectXMath.h>
#include <vector>

#include "Triangle.h"

#define NUM_CHILDREN 8

class Node
{
public:
	Node();
	Node(DirectX::XMFLOAT3 p_min, DirectX::XMFLOAT3 p_max);
	~Node();

	void setMin(DirectX::XMFLOAT3 p_min);
	void setMax(DirectX::XMFLOAT3 p_max);

	void addChild(unsigned int p_child);

	void subdivide(int p_levels, DirectX::XMFLOAT3 p_min, DirectX::XMFLOAT3 p_max);

private:
	bool triangleIntersect(Triangle p_tri);

	DirectX::XMFLOAT3 m_min;
	DirectX::XMFLOAT3 m_max;

	std::vector<unsigned int> m_children;
};

#endif //RAYTRACER_NODE_H