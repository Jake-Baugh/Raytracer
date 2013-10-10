#include "constantBuffers.hlsl"
#include "structs.hlsl"
#include "intersect.hlsl"

RWTexture2D<float4>				 output			: register(u0);
RWStructuredBuffer<Intersection> intersections	: register(u1);
RWStructuredBuffer<float4>		 accumulation	: register(u2);
RWStructuredBuffer<Ray>			 rays			: register(u3);

StructuredBuffer<Triangle>		 triangles		: register(t0);
StructuredBuffer<Light>			 lights			: register(t1);
Texture2D						 texCube		: register(t2);
StructuredBuffer<Material>		 materials		: register(t3);

SamplerState					 ssDefault		: register(s0);

float3 pointLight(float3 normal, float3 position, float4 diffuse, float4 specular, Light light)
{
	float3 litColor = float3(0.0f, 0.0f, 0.0f);

	float3 lightVec = position - light.pos;
	float d = length(lightVec);
	lightVec /= d;
	
	litColor += diffuse * light.ambient;
	
	float diffuseFactor = dot(lightVec, normal);
	if(diffuseFactor > 0.0f)
	{
		litColor += diffuseFactor * diffuse * light.diffuse;
	
		float specPower = max(specular.a, 1.0f);
		float3 toEye = normalize(cb_cameraPosition-position);
		float3 R = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(R, toEye), 0.0f), specPower);
	
		litColor += specFactor * specular * light.spec;
	}
	
	litColor /= dot(light.att, float3(1.0f, d, d*d));

	return litColor;
}

void reflectRay(uint index, float3 position, float3 normal, int triangleId )
{
	Ray ray					= rays[index];
	ray.m_origin			= position;
	ray.m_direction			= reflect(ray.m_direction, normal);
	ray.m_direction			= normalize(ray.m_direction);
	ray.m_triangleId		= triangleId;
	ray.m_reflectiveFactor *= materials[triangles[triangleId].m_mtlIndex].m_specular;//0.3f;
	rays[index]				= ray;
}

[numthreads(32, 32, 1)]
void main( uint3 threadID : SV_DispatchThreadID, uint groupID : SV_GroupID )
{	
	uint index =  threadID.y * cb_screenWidth + threadID.x;

	Intersection intersection = intersections[index];	
	
	Vertex vertexA;
	Vertex vertexB;
	Vertex vertexC;

	float4 color				= float4(0.0f, 0.0f, 0.0f, 1.0f);
	float3 litColor				= float3(0.0f, 0.0f, 0.0f);
	float2 barCoord				= float2(0.0f, 0.0f);
	float3 intersectPosition	= float3(0.0f, 0.0f, 0.0f);
	float3 intersectNormal		= float3(0.0f, 0.0f, 0.0f);
	float2 intersectTexture		= float2(0.0f, 0.0f);

	if(intersection.m_triangleId >= 0)
	{
		vertexA = triangles[intersection.m_triangleId].m_vertices[0];
		vertexB = triangles[intersection.m_triangleId].m_vertices[1];
		vertexC = triangles[intersection.m_triangleId].m_vertices[2];

		barCoord = intersection.m_barCoord;

		intersectPosition =	(1 - barCoord.x - barCoord.y) * vertexA.m_position +
							barCoord.x * vertexB.m_position +
							barCoord.y * vertexC.m_position;
		intersectNormal =	(1 - barCoord.x - barCoord.y) * vertexA.m_normal +
							barCoord.x * vertexB.m_normal +
							barCoord.y * vertexC.m_normal;
		intersectTexture =	(1 - barCoord.x - barCoord.y) * vertexA.m_texCoord +
										barCoord.x * vertexB.m_texCoord +
									barCoord.y * vertexC.m_texCoord;
		intersectNormal = normalize(intersectNormal);

		//reflectRay(index, intersectPosition, intersectNormal, intersection.m_triangleId);

		color			= texCube.SampleLevel(ssDefault, intersectTexture, 0);
		float4 diffuse	= materials[triangles[intersection.m_triangleId].m_mtlIndex].m_diffuse; //float4(1.0f, 1.0f, 1.0f, 1.0f);
		float4 specular	= materials[triangles[intersection.m_triangleId].m_mtlIndex].m_specular; //float4(1.0f, 1.0f, 1.0f, 0.0f);
		
		specular.a *= 256.0f;

		bool inShadow = false;
		for(uint lightIndex = 0; lightIndex < cb_numLights; lightIndex++)
		{	
			Light light = lights[lightIndex];
			light.pos *= -1.0f;
			
			Ray lightRay;
			lightRay.m_origin			= intersectPosition;
			lightRay.m_direction		= normalize(light.pos - intersectPosition);
			lightRay.m_maxDistance		= length(light.pos - intersectPosition);
			lightRay.m_triangleId		= intersection.m_triangleId;
			lightRay.m_reflectiveFactor	= float3(1.0f, 1.0f, 1.0f);
			lightRay.m_padding			= 1;

			for(int i=0; i<cb_numTriangles; i++)
			{
				float3 pos1 = triangles[i].m_vertices[0].m_position;
				float3 pos2 = triangles[i].m_vertices[1].m_position;
				float3 pos3 = triangles[i].m_vertices[2].m_position;
				Intersection shadowIntersection = intersectTriangle(lightRay, pos1, pos2, pos3);

				if( lightRay.m_triangleId != i &&
					shadowIntersection.m_barCoord.x >= 0.0f &&
					shadowIntersection.m_range > 0.0001f)
				{
						inShadow = true;
				}
			}
			if(inShadow)
			{
				litColor += lights[lightIndex].ambient;
			}
			else
			{
				litColor += pointLight(intersectNormal, intersectPosition, diffuse, specular, light);
			}
		}
	}
	
	accumulation[index] += color * float4(litColor, 1.0f) * float4(rays[index].m_reflectiveFactor, 1.0f);
	reflectRay(index, intersectPosition, intersectNormal, intersection.m_triangleId);

	output[threadID.xy] = accumulation[index];
}