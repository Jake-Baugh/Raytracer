#include "constantBuffers.hlsl"
#include "structs.hlsl"
#include "intersect.hlsl"

RWStructuredBuffer<Ray>			 rays			: register(u0);
RWStructuredBuffer<Intersection> intersections	: register(u1);

StructuredBuffer<Vertex> vertices : register(t0);

[numthreads(32, 32, 1)]
void main( uint3 threadID : SV_DispatchThreadID, uint groupID : SV_GroupID )
{
	uint index = threadID.y * cb_screenWidth + threadID.x;

	Ray ray = rays[index];

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
		for(uint i=0; i<cb_numVertices; i+=3)
		{	
			pos1 = vertices[i].m_position;
			pos2 = vertices[i+1].m_position;
			pos3 = vertices[i+2].m_position;
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
	intersections[index] = bestIntersection;
}