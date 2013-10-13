#ifndef RAYTRACER_INTERSECT_HLSL
#define RAYTRACER_INTERSECT_HLSL

#include "structs.hlsl"

#define FLT_MAX 3.402823466e+38F

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

bool intersectBox(Ray ray, Node node)
{
	float tMax = FLT_MAX;
	float tMin = -FLT_MAX;

	float3 ac = node.m_min + ((node.m_max - node.m_min)/2);
	float3 a[3];
	a[0] = float3(1.0f, 0.0f, 0.0f);
	a[1] = float3(0.0f, 1.0f, 0.0f);
	a[2] = float3(0.0f, 0.0f, 1.0f);
	float h[3];
	h[0] = (node.m_max.x - node.m_min.x)/2;
	h[1] = (node.m_max.y - node.m_min.y)/2;
	h[2] = (node.m_max.z - node.m_min.z)/2;

	float3 p = ac - ray.m_origin;

	float e;
	float f;
	float t1;
	float t2;
	float temp;

	for(int i=0; i<3; i++)
	{
		e = dot(a[i], p);
		f = dot(a[i], ray.m_direction);
		if(sqrt(f*f) > tMin)
		{
			t1 = (e + h[i])/f;
			t2 = (e - h[i])/f;
			if(t1 > t2)
			{
				temp = t1;
				t1 = t2;
				t2 = temp;
			}
			if(t1 > tMin)
				tMin = t1;
			if(t2 < tMax)
				tMax = t2;
			if(tMin > tMax)
				return false;
			if(tMax < 0)
				return false;
		}
		else if((-e-h[i])>0 || -e+h[i]<0)
			return false;
	}
	return true;
}

#endif //RAYTRACER_INTERSECT_HLSL