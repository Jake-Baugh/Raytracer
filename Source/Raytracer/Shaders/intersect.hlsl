#ifndef RAYTRACER_INTERSECT_HLSL
#define RAYTRACER_INTERSECT_HLSL

#include "structs.hlsl"

bool intersectSphere(Ray ray, Sphere sphere)
{
	bool intersect = false;
	float b = dot(ray.m_direction, ray.m_origin-sphere.m_center);
	float c = dot(ray.m_origin-sphere.m_center, ray.m_origin-sphere.m_center) - (sphere.m_radius * sphere.m_radius);

	float f = b*b - c;

	if(f >= 0)
		intersect = true;

	return intersect;
}

Intersection intersectTriangle(Ray ray, float3 pos1, float3 pos2, float3 pos3)
{
	Intersection intersection;
	intersection.m_barCoord = float2(-1.0f, -1.0f);
	intersection.m_range = -1.0f;
	intersection.m_triangleId = -1;

	bool intersect = true;

	float3 e1 = pos2 - pos1;
	float3 e2 = pos3 - pos1;
	float3 q  = cross(ray.m_direction, e2);
	
	float a = dot(e1, q);
	if(a > -0.00001f && a < 0.00001)
		intersect = false;
		
	float f = 1/a;
	float3 s = ray.m_origin - pos1;
	float u =  f*(dot(s, q));
	
	if(u < 0.0f)
		intersect = false;
	
	float3 r = cross(s, e1);
	float v = f*(dot(ray.m_direction, r));
	
	if(v < 0.0f || v+u > 1.0f)
		intersect = false;
	
	if( intersect )
	{
		intersection.m_barCoord = float2(u, v);
		intersection.m_range = dot(e2, r) * f;
	}

	return intersection;
}

#endif //RAYTRACER_INTERSECT_HLSL