#ifndef RAYTRACER_STRUCTS_HLSL
#define RAYTRACER_STRUCTS_HLSL

struct Ray
{
	float3	m_origin;
	float	m_maxDistance;
	float3	m_direction;
	int		m_triangleId;
	float3	m_reflectiveFactor;
	int		m_padding;
};

struct Intersection
{
	float2	m_barCoord;
	float	m_range;
	int		m_triangleId;
};

struct Vertex
{
	float3 m_position;
	float3 m_normal;
	float2 m_texCoord;
};

struct Sphere
{
	float3	m_center;
	float	m_radius;
};

struct Light
{
	float3	pos;
	float	spotPower;
	float3	dir;
	float	range;
	float4	ambient;
	float4	diffuse;
	float4	spec;
	float3	att;
	float	lightType;
	
};

#endif //RAYTRACER_STRUCTS_HLSL