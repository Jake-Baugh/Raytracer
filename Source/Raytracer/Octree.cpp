#include "Octree.h"

Octree::Octree()
{
}
Octree::~Octree()
{
}

void Octree::init(std::vector<Triangle> p_triangles)
{
	float minX = FLT_MAX;
	float minY = FLT_MAX;
	float minZ = FLT_MAX;

	float maxX = -FLT_MAX;
	float maxY = -FLT_MAX;
	float maxZ = -FLT_MAX;

	for(unsigned int triIdx=0; triIdx<p_triangles.size(); triIdx++)
	{
		for(unsigned int i=0; i<3; i++)
		{
			DirectX::XMFLOAT3 pos = p_triangles[triIdx].vertices[i].m_position;

			if(pos.x > maxX)
				maxX = pos.x;
			if(pos.y > maxY)
				maxY = pos.y;
			if(pos.z > maxZ)
				maxZ = pos.z;
			
			if(pos.x < minX)
				minX = pos.x;
			if(pos.y < minY)
				minY = pos.y;
			if(pos.z < minZ)
				minZ = pos.z;
		}
	}

	DirectX::XMFLOAT3 min = DirectX::XMFLOAT3(minX, minY, minZ);
	DirectX::XMFLOAT3 max = DirectX::XMFLOAT3(maxX, maxY, maxZ);

	m_nodes.push_back(Node(min, max));
	subdivide(min, max, 0, 2);

}

std::vector<Node>& Octree::getNodes()
{
	return m_nodes;
}

void Octree::subdivide(DirectX::XMFLOAT3 p_min, DirectX::XMFLOAT3 p_max, unsigned int p_currentNode, unsigned int p_levels)
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
	
		for(unsigned int i=0; i<8; i++)
		{
			m_nodes.push_back(Node(minValues[i], maxValues[i]));
			m_nodes[p_currentNode].addChild(m_nodes.size()-1);
			subdivide(minValues[i], maxValues[i], m_nodes.size()-1, p_levels-1);
		}		
	}
}