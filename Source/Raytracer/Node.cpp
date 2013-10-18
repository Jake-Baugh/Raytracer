#include <math.h>

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
	DirectX::XMFLOAT3 boxCenter	  = div(add(m_max, m_min), 2);
	DirectX::XMFLOAT3 boxHalfSize = div(sub(m_max, m_min), 2);
	
	//Move everything so that boxCenter is in (0, 0, 0)
	DirectX::XMFLOAT3 vertex0 = sub(p_tri.vertices[0].m_position, boxCenter);
	DirectX::XMFLOAT3 vertex1 = sub(p_tri.vertices[1].m_position, boxCenter);
	DirectX::XMFLOAT3 vertex2 = sub(p_tri.vertices[2].m_position, boxCenter);
	
	//Compute triangle edges
	DirectX::XMFLOAT3 edge0 = sub(vertex1, vertex0);
	DirectX::XMFLOAT3 edge1 = sub(vertex2, vertex1);
	DirectX::XMFLOAT3 edge2 = sub(vertex0, vertex2);

	float fex = fabsf(edge0.x);
	float fey = fabsf(edge0.y);
	float fez = fabsf(edge0.z);
	if(axisTestX(edge0.z, edge0.y, fez, fey, vertex0, vertex2, boxHalfSize) == false)
		return false;
	if(axisTestY(edge0.z, edge0.x, fez, fex, vertex0, vertex2, boxHalfSize) == false)
		return false;
	if(axisTestZ(edge0.y, edge0.x, fey, fex, vertex1, vertex2, boxHalfSize) == false)
		return false;

	fex = fabsf(edge1.x);
	fey = fabsf(edge1.y);
	fez = fabsf(edge1.z);
	if(axisTestX(edge1.z, edge1.y, fez, fey, vertex0, vertex2, boxHalfSize) == false)
		return false;
	if(axisTestY(edge1.z, edge1.x, fez, fex, vertex0, vertex2, boxHalfSize) == false)
		return false;
	if(axisTestZ(edge1.y, edge1.x, fey, fex, vertex0, vertex1, boxHalfSize) == false)
		return false;

	fex = fabsf(edge2.x);
	fey = fabsf(edge2.y);
	fez = fabsf(edge2.z);
	if(axisTestX(edge2.z, edge2.y, fez, fey, vertex0, vertex1, boxHalfSize) == false)
		return false;
	if(axisTestY(edge2.z, edge2.x, fez, fex, vertex0, vertex1, boxHalfSize) == false)
		return false;
	if(axisTestZ(edge2.y, edge2.x, fey, fex, vertex1, vertex2, boxHalfSize) == false)
		return false;

	float min, max;
	findMinMax(vertex0.x, vertex1.x, vertex2.x, min, max);
	if(min > boxHalfSize.x || max < -boxHalfSize.x)
		return false;

	findMinMax(vertex0.y, vertex1.y, vertex2.y, min, max);
	if(min > boxHalfSize.y || max < -boxHalfSize.y)
		return false;

	findMinMax(vertex0.z, vertex1.z, vertex2.z, min, max);
	if(min > boxHalfSize.z || max < -boxHalfSize.z)
		return false;

	DirectX::XMFLOAT3 normal = cross(edge0, edge1);
	if(planeBoxOverlap(normal, vertex0, boxHalfSize) == false)
		return false;

	return true;
}

bool LinkedNode::planeBoxOverlap(DirectX::XMFLOAT3 p_normal, DirectX::XMFLOAT3 p_vert, DirectX::XMFLOAT3 p_maxBox)
{
	float normal[3] = {p_normal.x, p_normal.y, p_normal.z};
	float vert[3]	= {p_vert.x, p_vert.y, p_vert.z};
	float maxBox[3] = {p_maxBox.x, p_maxBox.y, p_maxBox.z};

	float vMin[3], vMax[3], v;
	for(unsigned int i=0; i<3; i++)
	{
		v = vert[i];
		if(normal[i] > 0.0f)
		{
			vMin[i] = -maxBox[i] - v;
			vMax[i] = maxBox[i] - v;
		}
		else
		{
			vMin[i] = maxBox[i] - v;
			vMax[i] = -maxBox[i] - v;
		}
	}

	if(dot(DirectX::XMFLOAT3(normal), DirectX::XMFLOAT3(vMin)) > 0.0f)
		return false;
	if(dot(DirectX::XMFLOAT3(normal), DirectX::XMFLOAT3(vMax)) >= 0.0f)
		return true;

	return false;
}

bool LinkedNode::axisTestX(float p_a, float p_b, float p_fa, float p_fb,
							  DirectX::XMFLOAT3 p_v0, DirectX::XMFLOAT3 p_v1, DirectX::XMFLOAT3 p_boxHalfSize)
{
	bool intersect = true;
	float p0, p1, min, max, rad;
	p0 = p_a*p_v0.y - p_b*p_v0.z;
	p1 = p_a*p_v1.y - p_b*p_v1.z;

	if(p0<p1)
	{
		min = p0;
		max = p1;
	}
	else
	{
		min = p1;
		max = p0;
	}
	rad = p_fa * p_boxHalfSize.y + p_fb * p_boxHalfSize.z;
	if(min>rad || max<-rad)
		intersect = false;

	return intersect;
}
bool LinkedNode::axisTestY(float p_a, float p_b, float p_fa, float p_fb,
							  DirectX::XMFLOAT3 p_v0, DirectX::XMFLOAT3 p_v1, DirectX::XMFLOAT3 p_boxHalfSize)
{
	bool intersect = true;
	float p0, p1, min, max, rad;
	p0 = -p_a*p_v0.x + p_b*p_v0.z;
	p1 = -p_a*p_v1.x + p_b*p_v1.z;

	if(p0<p1)
	{
		min = p0;
		max = p1;
	}
	else
	{
		min = p1;
		max = p0;
	}
	rad = p_fa * p_boxHalfSize.x + p_fb * p_boxHalfSize.z;
	if(min>rad || max<-rad)
		intersect = false;

	return intersect;
}
bool LinkedNode::axisTestZ(float p_a, float p_b, float p_fa, float p_fb,
							  DirectX::XMFLOAT3 p_v0, DirectX::XMFLOAT3 p_v1, DirectX::XMFLOAT3 p_boxHalfSize)
{
	bool intersect = true;
	float p0, p1, min, max, rad;
	p0 = p_a*p_v0.x - p_b*p_v0.y;
	p1 = p_a*p_v1.x - p_b*p_v1.y;

	if(p0<p1)
	{
		min = p0;
		max = p1;
	}
	else
	{
		min = p1;
		max = p0;
	}
	rad = p_fa * p_boxHalfSize.x + p_fb * p_boxHalfSize.y;
	if(min>rad || max<-rad)
		intersect = false;

	return intersect;
}

void LinkedNode::findMinMax(float p_x0, float p_x1, float p_x2, float& io_min, float& io_max)
{
	io_min = io_max = p_x0;
	if(p_x1 < io_min)
		io_min =  p_x1;
	if(p_x1 > io_max)
		io_max = p_x1;
	if(p_x2 < io_min)
		io_min =  p_x2;
	if(p_x2 > io_max)
		io_max = p_x2;
}

DirectX::XMFLOAT3 LinkedNode::sub(DirectX::XMFLOAT3 p_v1, DirectX::XMFLOAT3 p_v2)
{
	DirectX::XMFLOAT3 res;
	res.x = p_v1.x - p_v2.x;
	res.y = p_v1.y - p_v2.y;
	res.z = p_v1.z - p_v2.z;
	return res;
}
DirectX::XMFLOAT3 LinkedNode::add(DirectX::XMFLOAT3 p_v1, DirectX::XMFLOAT3 p_v2)
{
	DirectX::XMFLOAT3 res;
	res.x = p_v1.x + p_v2.x;
	res.y = p_v1.y + p_v2.y;
	res.z = p_v1.z + p_v2.z;
	return res;
}
DirectX::XMFLOAT3 LinkedNode::mul(DirectX::XMFLOAT3 p_v, float p_scalar)
{
	DirectX::XMFLOAT3 res;
	res.x = p_v.x * p_scalar;
	res.y = p_v.y * p_scalar;
	res.z = p_v.z * p_scalar;
	return res;
}
DirectX::XMFLOAT3 LinkedNode::div(DirectX::XMFLOAT3 p_v, float p_scalar)
{
	DirectX::XMFLOAT3 res;
	res.x = p_v.x / p_scalar;
	res.y = p_v.y / p_scalar;
	res.z = p_v.z / p_scalar;
	return res;
}

DirectX::XMFLOAT3 LinkedNode::cross(DirectX::XMFLOAT3 p_v1, DirectX::XMFLOAT3 p_v2)
{
	DirectX::XMVECTOR xmV1, xmV2, xmRes;
	xmV1  = DirectX::XMLoadFloat3(&p_v1);
	xmV2  = DirectX::XMLoadFloat3(&p_v2);
	xmRes = DirectX::XMVector3Cross(xmV1, xmV2);

	DirectX::XMFLOAT3 res;
	DirectX::XMStoreFloat3(&res, xmRes);
	return res;
}
float LinkedNode::dot(DirectX::XMFLOAT3 p_v1, DirectX::XMFLOAT3 p_v2)
{
	DirectX::XMVECTOR xmV1, xmV2, xmRes;
	xmV1  = DirectX::XMLoadFloat3(&p_v1);
	xmV2  = DirectX::XMLoadFloat3(&p_v2);
	xmRes = DirectX::XMVector3Dot(xmV1, xmV2);

	DirectX::XMFLOAT3 res;
	DirectX::XMStoreFloat3(&res, xmRes);
	return res.x;
}