#include "Node.h"
#include "Octree.h"
#include "Utility.h"

Node::Node()
{
	numTris = 0;
	for(unsigned int i=0; i<MAX_NUM_TRIS; i++)
		triIndices[i] = -1;
	for(unsigned int i=0; i<NUM_CHILDREN; i++)
		children[i] = -1;
}
Node::Node(DirectX::XMFLOAT3 p_min,
		   DirectX::XMFLOAT3 p_max,
		   std::vector<unsigned int> p_triIndices,
		   std::vector<unsigned int> p_children)
{
	min = p_min;
	max = p_max;

	numTris = p_triIndices.size();
	for(unsigned int i=0; i<MAX_NUM_TRIS; i++)
	{
		if(i<numTris)
			triIndices[i] = p_triIndices[i];
		else
			triIndices[i] = -1;
	}
	for(unsigned int i=0; i<NUM_CHILDREN; i++)
	{
		if(i<p_children.size())
			children[i] = p_children[i];
		else
			children[i] = -1;
	}
}

LinkedNode::LinkedNode()
{
	m_min = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_max = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	
	for(unsigned int i=0; i<NUM_CHILDREN; i++)
		m_children[i] = nullptr;
}
LinkedNode::LinkedNode(DirectX::XMFLOAT3 p_min, DirectX::XMFLOAT3 p_max)
{
	m_min = p_min;
	m_max = p_max;

	for(unsigned int i=0; i<NUM_CHILDREN; i++)
		m_children[i] = nullptr;
}
LinkedNode::~LinkedNode()
{
	for(unsigned int i=0; i<NUM_CHILDREN; i++)
		SAFE_DELETE(m_children[i]);
}

void LinkedNode::setMin(DirectX::XMFLOAT3 p_min)
{
	m_min = p_min;
}
void LinkedNode::setMax(DirectX::XMFLOAT3 p_max)
{
	m_max = p_max;
}

DirectX::XMFLOAT3 LinkedNode::getMin()
{
	return m_min;
}
DirectX::XMFLOAT3 LinkedNode::getMax()
{
	return m_max;
}

void LinkedNode::addChildIndex(unsigned int p_index)
{
	m_childIndices.push_back(p_index);
}

LinkedNode* LinkedNode::getChild(unsigned int p_index)
{
	LinkedNode* node = nullptr;
	if(p_index >= 0 && p_index < NUM_CHILDREN)
		node = m_children[p_index];
	return node;
}

std::vector<unsigned int> LinkedNode::getTriIndices()
{
	return m_triIndices;
}
std::vector<unsigned int> LinkedNode::getChildIndices()
{
	return m_childIndices;
}

void LinkedNode::subdivide(DirectX::XMFLOAT3 p_min,
					 DirectX::XMFLOAT3 p_max,
					 std::vector<unsigned int> p_indices,
					 std::vector<Triangle> p_triangles)
{
	std::vector<unsigned int> triIndices;
	bool intersect = false;
	for(unsigned int i=0; i<p_indices.size(); i++)
	{
		intersect = triangleIntersect(p_triangles[p_indices[i]]);
		if(intersect)
			triIndices.push_back(p_indices[i]);
	}

	if(triIndices.size() > MAX_NUM_TRIS)
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
			m_children[i] = new LinkedNode(minValues[i], maxValues[i]);
			m_children[i]->subdivide(minValues[i], maxValues[i], triIndices, p_triangles);
		}
	}
	else
		m_triIndices = triIndices;
}

bool LinkedNode::triangleIntersect(Triangle p_tri)
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