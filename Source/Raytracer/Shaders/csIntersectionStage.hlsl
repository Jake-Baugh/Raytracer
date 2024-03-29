#include "constantBuffers.hlsl"
#include "structs.hlsl"
#include "intersect.hlsl"

RWStructuredBuffer<Ray>			 rays			: register(u0);
RWStructuredBuffer<Intersection> intersections	: register(u1);

StructuredBuffer<Triangle>	triangles	: register(t0);
StructuredBuffer<Node>		nodes		: register(t1);


Intersection intersectTest(Ray ray, uint index)
{
	float minDistance = ray.m_maxDistance;
	
	bool intersect = false;
	Intersection intersection;
	Intersection bestIntersection;
	bestIntersection.m_barCoord = float2(-1.0f, -1.0f);
	bestIntersection.m_range = ray.m_maxDistance;
	bestIntersection.m_triangleId = -1;

	float3 pos1, pos2, pos3;
	if(ray.m_triangleId > -2)
	{
		for(int i=0; i<cb_numTriangles; i++)
		{	
			pos1 = triangles[i].m_vertices[0].m_position;
			pos2 = triangles[i].m_vertices[1].m_position;
			pos3 = triangles[i].m_vertices[2].m_position;
			intersection = intersectTriangle(ray, pos1, pos2, pos3);
			
			if(	ray.m_triangleId != i &&
				intersection.m_barCoord.x >= 0.0f &&
				intersection.m_range < bestIntersection.m_range &&
				intersection.m_range > 0.0f)
			{
				bestIntersection = intersection;
				bestIntersection.m_triangleId = i;
				intersect = true;
			}
		}
		if(!intersect)
		{
			bestIntersection.m_triangleId = -2;
			rays[index].m_triangleId = -2;
		}
	}

	return bestIntersection;
}

Intersection intersectTestAccelerated(Ray ray, uint index)
{
	Intersection intersection;
	Intersection bIntersection;
	bIntersection.m_barCoord = float2(-1.0f, -1.0f);
	bIntersection.m_range = ray.m_maxDistance;
	bIntersection.m_triangleId = -2;

	float3 vfInvDir = float3(1.0f / ray.m_direction);

	int currentNode = 0;
	int stackOffset = 0;
	int stack[128];
	stack[0] = 0;

	bool intersect = false;
	while(true)
	{
		currentNode = stack[stackOffset];
		stackOffset--;
		Node node = nodes[currentNode];

		float2 T = BVH_IntersectBox(ray.m_origin, vfInvDir, node);

		if(T[0] > bIntersection.m_range || T[1] < 0.0f)
		{
			//Ray misses
		}
		else //if(intersectBox(ray, node))
		{
			if(nodes[currentNode].m_children[0] != -1)
			{
				for(uint i=0; i<NUM_CHILDREN; i++)
				{	
					stackOffset++;
					stack[stackOffset] = node.m_children[i];
				}
			}
			else //leaf node
			{
				float3 pos1, pos2, pos3;
				if(ray.m_triangleId > -2)
				{
					for(int i=0; i<node.m_numTris; i++)
					{	
						pos1 = triangles[node.m_triIndices[i]].m_vertices[0].m_position;
						pos2 = triangles[node.m_triIndices[i]].m_vertices[1].m_position;
						pos3 = triangles[node.m_triIndices[i]].m_vertices[2].m_position;
						intersection = intersectTriangle(ray, pos1, pos2, pos3);
						
						if(	ray.m_triangleId != node.m_triIndices[i] &&
							intersection.m_barCoord.x >= 0.0f &&
							intersection.m_range < bIntersection.m_range &&
							intersection.m_range > FLT_MIN &&
							intersection.m_triangleId != bIntersection.m_triangleId)
						{
							bIntersection = intersection;
							bIntersection.m_triangleId = node.m_triIndices[i];
							intersect = true;
						}
					}
				}
				
			}
		}
	//	stackOffset--;
	//	if(stackOffset >= 0)
	//		currentNode = stack[stackOffset];
		if(stackOffset < 0)
			break;
	}
	if(!intersect)
	{
		bIntersection.m_triangleId = -2;
		rays[index].m_triangleId = -2;
	}
	return bIntersection;
}

[numthreads(BLOCK_SIZE_X, BLOCK_SIZE_Y, 1)]
void main( uint3 threadID : SV_DispatchThreadID, uint groupID : SV_GroupID )
{
	uint index = threadID.y * cb_screenWidth + threadID.x;

	Ray ray = rays[index];
	
	intersections[index] = intersectTestAccelerated(ray, index);

	//intersections[index] = intersectTest(ray, index);
}