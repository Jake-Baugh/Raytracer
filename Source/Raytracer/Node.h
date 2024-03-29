#ifndef RAYTRACER_NODE_H
#define RAYTRACER_NODE_H

#include <DirectXMath.h>
#include <vector>

#include "Triangle.h"

#define NUM_CHILDREN 8
#define MAX_NUM_TRIS 10

struct Node
{
	DirectX::XMFLOAT3 min;
	DirectX::XMFLOAT3 max;
	
	unsigned int numTris;
	int triIndices[MAX_NUM_TRIS];
	int children[NUM_CHILDREN];

	Node();
	Node(DirectX::XMFLOAT3 p_min,
		 DirectX::XMFLOAT3 p_max,
		 std::vector<unsigned int> p_triIndices,
		 std::vector<unsigned int> p_children);
};

class LinkedNode
{
public:
	LinkedNode();
	LinkedNode(DirectX::XMFLOAT3 p_min, DirectX::XMFLOAT3 p_max);
	~LinkedNode();

	void setMin(DirectX::XMFLOAT3 p_min);
	void setMax(DirectX::XMFLOAT3 p_max);

	DirectX::XMFLOAT3 getMin();
	DirectX::XMFLOAT3 getMax();

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
	bool planeBoxOverlap(DirectX::XMFLOAT3 p_normal, DirectX::XMFLOAT3 p_vert, DirectX::XMFLOAT3 p_maxBox);

	bool axisTestX(float p_a, float p_b, float p_fa, float p_fb,
			DirectX::XMFLOAT3 p_v0, DirectX::XMFLOAT3 p_v1, DirectX::XMFLOAT3 p_boxHalfSize); 

	bool axisTestY(float p_a, float p_b, float p_fa, float p_fb,
			DirectX::XMFLOAT3 p_v0, DirectX::XMFLOAT3 p_v1, DirectX::XMFLOAT3 p_boxHalfSize);

	bool axisTestZ(float p_a, float p_b, float p_fa, float p_fb,
			DirectX::XMFLOAT3 p_v0, DirectX::XMFLOAT3 p_v1, DirectX::XMFLOAT3 p_boxHalfSize);

	void findMinMax(float p_x0, float p_x1, float p_x2, float& io_min, float& io_max);

	DirectX::XMFLOAT3 sub(DirectX::XMFLOAT3 p_v1, DirectX::XMFLOAT3 p_v2);
	DirectX::XMFLOAT3 add(DirectX::XMFLOAT3 p_v1, DirectX::XMFLOAT3 p_v2);
	DirectX::XMFLOAT3 mul(DirectX::XMFLOAT3 p_v1, float p_scalar);
	DirectX::XMFLOAT3 div(DirectX::XMFLOAT3 p_v1, float p_scalar);
	DirectX::XMFLOAT3 cross(DirectX::XMFLOAT3 p_v1, DirectX::XMFLOAT3 p_v2);
	float			  dot(DirectX::XMFLOAT3 p_v1, DirectX::XMFLOAT3 p_v2);

	DirectX::XMFLOAT3 m_min;
	DirectX::XMFLOAT3 m_max;

	std::vector<unsigned int> m_triIndices;
	std::vector<unsigned int> m_childIndices;

	LinkedNode* m_children[NUM_CHILDREN];
};

#endif //RAYTRACER_NODE_H