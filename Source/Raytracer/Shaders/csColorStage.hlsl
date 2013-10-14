#include "constantBuffers.hlsl"
#include "structs.hlsl"
#include "intersect.hlsl"

RWTexture2D<float4>				 output			: register(u0);
RWStructuredBuffer<Intersection> intersections	: register(u1);
RWStructuredBuffer<float4>		 accumulation	: register(u2);
RWStructuredBuffer<Ray>			 rays			: register(u3);

StructuredBuffer<Triangle>	triangles	: register(t0);
StructuredBuffer<Light>		lights		: register(t1);
Texture2DArray				texCube		: register(t2);
StructuredBuffer<Material>	materials	: register(t3);
StructuredBuffer<Node>		nodes		: register(t4);

SamplerState ssDefault : register(s0);

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

/***************************************
********** Intersection tests **********
***************************************/
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

	return bestIntersection;
}

Intersection intersectTestAccelerated(Ray ray, uint index)
{
	Intersection intersection;
	Intersection bIntersection;
	bIntersection.m_barCoord = float2(-1.0f, -1.0f);
	bIntersection.m_range = ray.m_maxDistance;
	bIntersection.m_triangleId = -2;

	int currentNode = 0;
	int stackOffset = 0;
	int stack[128];
	stack[0] = 0;

	bool intersect = false;
	while(true)
	{
		if(intersectBox(ray, nodes[currentNode]))
		{
			if(nodes[currentNode].m_children[0] != -1)
			{
				for(uint i=0; i<NUM_CHILDREN; i++)
				{
					stack[stackOffset] = nodes[currentNode].m_children[i];
					stackOffset++;
				}
			}
			else //leaf node
			{
				float3 pos1, pos2, pos3;
				for(int i=0; i<nodes[currentNode].m_numTris; i++)
				{	
					pos1 = triangles[nodes[currentNode].m_triIndices[i]].m_vertices[0].m_position;
					pos2 = triangles[nodes[currentNode].m_triIndices[i]].m_vertices[1].m_position;
					pos3 = triangles[nodes[currentNode].m_triIndices[i]].m_vertices[2].m_position;
					intersection = intersectTriangle(ray, pos1, pos2, pos3);
					
					if(	ray.m_triangleId != nodes[currentNode].m_triIndices[i] &&
						intersection.m_barCoord.x >= 0.0f &&
						intersection.m_range < bIntersection.m_range &&
						intersection.m_range > 0.00001f)
					{
						bIntersection = intersection;
						bIntersection.m_triangleId = nodes[currentNode].m_triIndices[i];
						intersect = true;
					}
				}
			}
		}
		stackOffset--;
		if(stackOffset >= 0)
			currentNode = stack[stackOffset];
		else
			break;
	}
	if(!intersect)
	{
		bIntersection.m_triangleId = -2;
		rays[index].m_triangleId = -2;
	}
	return bIntersection;
}
/*******************************************
********** Intersection tests end **********
********************************************/



[numthreads(BLOCK_SIZE_X, BLOCK_SIZE_Y, 1)]
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

		color			= texCube.SampleLevel(ssDefault, float3(intersectTexture, triangles[intersection.m_triangleId].m_mtlIndex), 0);
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

			//for(int i=0; i<cb_numTriangles; i++)
			//{
			//	float3 pos1 = triangles[i].m_vertices[0].m_position;
			//	float3 pos2 = triangles[i].m_vertices[1].m_position;
			//	float3 pos3 = triangles[i].m_vertices[2].m_position;
			//	Intersection shadowIntersection = intersectTriangle(lightRay, pos1, pos2, pos3);
			//
			//	if( lightRay.m_triangleId != i &&
			//		shadowIntersection.m_barCoord.x >= 0.0f &&
			//		shadowIntersection.m_range > 0.0001f)
			//	{
			//			inShadow = true;
			//	}
			//}
			
			Intersection shadowIntersection = intersectTestAccelerated(lightRay, index);
			//Intersection shadowIntersection = intersectTest(lightRay, index);
			if( //lightRay.m_triangleId != shadowIntersection.m_triangleId &&
				shadowIntersection.m_barCoord.x >= 0.0f &&
				shadowIntersection.m_range > 0.0001f)
			{
					inShadow = true;
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