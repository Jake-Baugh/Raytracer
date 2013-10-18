#ifndef RAYTRACER_STRUCTS_HLSL
#define RAYTRACER_STRUCTS_HLSL

#define BLOCK_SIZE_X 16
#define BLOCK_SIZE_Y 16
#define NUM_CHILDREN 8
#define MAX_NUM_TRIS 10

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

struct Triangle
{
	Vertex m_vertices[3];
	int m_mtlIndex;
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

struct Material
{
	float4 m_ambient;
	float4 m_diffuse;
	float4 m_specular;
};

struct Node
{
	float3 m_min;
	float3 m_max;

	uint m_numTris;
	int	 m_triIndices[MAX_NUM_TRIS];
	int  m_children[NUM_CHILDREN];
};

#endif //RAYTRACER_STRUCTS_HLSL