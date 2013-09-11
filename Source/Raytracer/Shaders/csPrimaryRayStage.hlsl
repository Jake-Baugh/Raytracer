#include "constantBuffers.hlsl"
#include "structs.hlsl"

RWStructuredBuffer<Ray>		rays		 : register(u0);
RWStructuredBuffer<float4>	accumulation : register(u1);

float3 calcPixelPosition(uint3 threadID)
{
	float screenX = threadID.x;
	float screenY = threadID.y;
	
	//float viewX = (((2.0f*screenX)/cb_screenWidth)-1.0f)/cb_projMatrix._11;
	//float viewY = (((-2.0f*screenY)/cb_screenHeight)+1.0f)/cb_projMatrix._22; 
	//float viewZ = 1.0f;
	//
	//float3 pixelPosition = float3(viewX, viewY, viewZ);
	//pixelPosition = mul(float4(pixelPosition, 1.0f), cb_viewMatrixInv).xyz;

	float inverse = 1.0f/float(cb_screenWidth);

	float y = -float(2.0f * threadID.y + 1.0f - cb_screenWidth) * inverse;
	float x = float(2.0f * threadID.x +1.0f - cb_screenWidth) * inverse;
	float z = 2.0f;

	float3 pixelPosition = mul(float4(x, y, z, 1.0f), cb_viewMatrixInv).xyz;

	return pixelPosition;
}

Ray createPrimaryRay(float3 pixelPosition)
{
	Ray ray;
	float4 aux				= mul(float4(0.0f, 0.0f, 0.0f, 1.0f), cb_viewMatrixInv);
	ray.m_origin			= aux.xyz/aux.w;
	ray.m_direction			= normalize(pixelPosition-ray.m_origin);
	ray.m_maxDistance		= 1000000000000000.0f;
	ray.m_reflectiveFactor	= float3(1.0f, 1.0f, 1.0f);
	ray.m_triangleId		= -1;
	ray.m_padding			= 0;

	return ray;
}

[numthreads(32, 32, 1)]
void main( uint3 threadID : SV_DispatchThreadID, uint groupID : SV_GroupID )
{	
	uint index = threadID.y * cb_screenWidth + threadID.x;
	accumulation[index] = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 pixelPosition = calcPixelPosition(threadID);
	Ray ray = createPrimaryRay(pixelPosition);

	rays[index] = ray;
}