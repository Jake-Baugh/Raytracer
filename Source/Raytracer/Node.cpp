#include "Node.h"
#include "Octree.h"
#include "Utility.h"

Node::Node()
{
	m_min = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_max = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	
	for(unsigned int i=0; i<NUM_CHILDREN; i++)
		m_children[i] = nullptr;
}
Node::Node(DirectX::XMFLOAT3 p_min, DirectX::XMFLOAT3 p_max)
{
	m_min = p_min;
	m_max = p_max;

	for(unsigned int i=0; i<NUM_CHILDREN; i++)
		m_children[i] = nullptr;
}
Node::~Node()
{
	for(unsigned int i=0; i<NUM_CHILDREN; i++)
		SAFE_DELETE(m_children[i]);
}

void Node::setMin(DirectX::XMFLOAT3 p_min)
{
	m_min = p_min;
}
void Node::setMax(DirectX::XMFLOAT3 p_max)
{
	m_max = p_max;
}

void Node::addPrimitive(unsigned int p_primitive)
{
	m_primitives.push_back(p_primitive);
}

void Node::subdivide(DirectX::XMFLOAT3 p_min,
					 DirectX::XMFLOAT3 p_max,
					 std::vector<unsigned int> p_indices,
					 std::vector<Triangle> p_triangles)
{
	std::vector<unsigned int> intersectIndices;
	bool intersect = false;
	for(unsigned int i=0; i<p_indices.size(); i++)
	{
		intersect = triangleIntersect(p_triangles[p_indices[i]]);
		if(intersect)
			intersectIndices.push_back(p_indices[i]);
	}

	if(intersectIndices.size() > MAX_NUM_TRIS)
	{
		DirectX::XMFLOAT3 minValues[8];
		DirectX::XMFLOAT3 maxValues[8];

		minValues[0] = p_min;
		maxValues[0] = DirectX::XMFLOAT3( (p_min.x+p_max.x)/2, (p_min.y+p_max.y)/2 , (p_min.z+p_max.z)/2 );

		minValues[1] = DirectX::XMFLOAT3( (p_min.x+p_max.x)/2, p_min.y, p_min.z );
		maxValues[1] = DirectX::XMFLOAT3( p_max.x, (p_min.y+p_max.y)/2 , (p_min.z+p_max.z)/2 );

		minValues[2] = DirectX::XMFLOAT3( p_min.x, p_min.y, (p_min.z+p_max.z)/2 );
		maxValues[2] = DirectX::XMFLOAT3( (p_min.x+p_max.x)/2, (p_min.y+p_max.y)/2 , p_max.z );

		minValues[3] = DirectX::XMFLOAT3( (p_min.x+p_max.x)/2, p_min.y, (p_min.z+p_max.z)/2 );
		maxValues[3] = DirectX::XMFLOAT3( p_max.x, (p_min.y+p_max.y)/2 , p_max.z );

		minValues[4] = DirectX::XMFLOAT3( p_min.x, (p_min.y+p_max.y)/2, p_min.z );
		maxValues[4] = DirectX::XMFLOAT3( (p_min.x+p_max.x)/2, p_max.y , (p_min.z+p_max.z)/2 );

		minValues[5] = DirectX::XMFLOAT3( (p_min.x+p_max.x)/2, (p_min.y+p_max.y)/2, p_min.z );
		maxValues[5] = DirectX::XMFLOAT3( p_max.x, p_max.y , (p_min.z+p_max.z)/2 );

		minValues[6] = DirectX::XMFLOAT3( p_min.x, (p_min.y+p_max.y)/2, (p_min.z+p_max.z)/2 );
		maxValues[6] = DirectX::XMFLOAT3( (p_min.x+p_max.x)/2, p_max.y, p_max.z );

		minValues[7] = DirectX::XMFLOAT3( (p_min.x+p_max.x)/2, (p_min.y+p_max.y)/2, (p_min.z+p_max.z)/2 );
		maxValues[7] = DirectX::XMFLOAT3( p_max.x, p_max.y, p_max.z );

		for(unsigned int i=0; i<NUM_CHILDREN; i++)
		{
			m_children[i] = new Node(minValues[i], maxValues[i]);
			m_children[i]->subdivide(minValues[i], maxValues[i], intersectIndices, p_triangles);
		}
	}
}

bool Node::triangleIntersect(Triangle p_tri)
{
	bool intersect = false;
	DirectX::XMFLOAT3 pos;
	for(unsigned int i=0; i<3; i++)
	{
		pos = p_tri.vertices[i].m_position;
		if(pos.x > m_min.x && pos.y > m_min.y && pos.z > m_min.z &&
			pos.x < m_max.x && pos.y < m_max.y && pos.z < m_max.z)
			intersect = true;
	}
	return intersect;
}