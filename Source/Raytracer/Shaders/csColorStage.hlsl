#include "constantBuffers.hlsl"
#include "structs.hlsl"
#include "intersect.hlsl"

RWTexture2D<float4>				 output			: register(u0);
RWStructuredBuffer<Intersection> intersections	: register(u1);
RWStructuredBuffer<float4>		 accumulation	: register(u2);
RWStructuredBuffer<Ray>			 rays			: register(u3);

StructuredBuffer<Vertex>		 vertices		: register(t0);
StructuredBuffer<Light>			 lights			: register(t1);
Texture2D						 texCube		: register(t2);

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
	Ray ray			 = rays[index];
	ray.m_origin	 = position;
	ray.m_direction  = reflect(ray.m_direction, normal);
	ray.m_direction	 = normalize(ray.m_direction);
	ray.m_triangleId = triangleId;
	rays[index]		 = ray;
}

[numthreads(32, 32, 1)]
void main( uint3 threadID : SV_DispatchThreadID, uint groupID : SV_GroupID )
{	
	uint index =  threadID.y * cb_screenWidth + threadID.x;

	Intersection intersection = intersections[index];	
	
	Vertex vertexA;
	Vertex vertexB;
	Vertex vertexC;

	float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float3 litColor = float3(0.0f, 0.0f, 0.0f);
	if(intersection.m_triangleId >= 0)
	{
		vertexA = vertices[intersection.m_triangleId];
		vertexB = vertices[intersection.m_triangleId+1];
		vertexC = vertices[intersection.m_triangleId+2];

		float2 barCoord = intersection.m_barCoord;

		float3 intersectPosition =	(1 - barCoord.x - barCoord.y) * vertexA.m_position +
									barCoord.x * vertexB.m_position +
									barCoord.y * vertexC.m_position;
		float3 intersectNormal =	(1 - barCoord.x - barCoord.y) * vertexA.m_normal +
									barCoord.x * vertexB.m_normal +
									barCoord.y * vertexC.m_normal;
		float2 intersectTexture =	(1 - barCoord.x - barCoord.y) * vertexA.m_texCoord +
									barCoord.x * vertexB.m_texCoord +
									barCoord.y * vertexC.m_texCoord;
		intersectNormal = normalize(intersectNormal);

		reflectRay(index, intersectPosition, intersectNormal, intersection.m_triangleId);

		color			= texCube.SampleLevel(ssDefault, intersectTexture, 0);
		float4 diffuse	= float4(1.0f, 1.0f, 1.0f, 1.0f);
		float4 specular	= float4(1.0f, 1.0f, 1.0f, 0.0f);
		
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

			for(int i=0; i<cb_numVertices; i+=3)
			{
				float3 pos1 = vertices[i].m_position;
				float3 pos2 = vertices[i+1].m_position;
				float3 pos3 = vertices[i+2].m_position;
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
	
	accumulation[index] += color * float4(litColor, 1.0f);
	output[threadID.xy] = accumulation[index];
}