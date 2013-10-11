#include "Node.h"
#include "Octree.h"

Node::Node()
{
	m_min = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_max = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_children.resize(0, 0);
}
Node::Node(DirectX::XMFLOAT3 p_min, DirectX::XMFLOAT3 p_max)
{
	m_min = p_min;
	m_max = p_max;
	m_children.resize(0, 0);
}
Node::~Node()
{
}

void Node::setMin(DirectX::XMFLOAT3 p_min)
{
	m_min = p_min;
}
void Node::setMax(DirectX::XMFLOAT3 p_max)
{
	m_max = p_max;
}

void Node::addChild(unsigned int p_child)
{
	m_children.push_back(p_child);
}

void Node::subdivide(int p_levels, DirectX::XMFLOAT3 p_min, DirectX::XMFLOAT3 p_max)
{
	if(p_levels > 0)
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
		
	}
}

bool Node::triangleIntersect(Triangle p_tri)
{
	bool intersect = true;
	DirectX::XMFLOAT3 pos;
	for(unsigned int i=0; i<3; i++)
	{
		pos = p_tri.vertices[i].m_position;
		if(pos.x < m_min.x || pos.y < m_min.y || pos.z < m_min.z)
			intersect = false;
		else if(pos.x > m_max.x || pos.y > m_max.y || pos.z > m_max.z)
			intersect = false;
	}
	return intersect;
}