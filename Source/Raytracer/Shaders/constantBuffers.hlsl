#ifndef RAYTRACER_CONSTANT_BUFFERS_HLSL
#define RAYTRACER_CONSTANT_BUFFERS_HLSL

#pragma pack_matrix(row_major)

cbuffer cbFrame : register( b0 )
{
	row_major float4x4	cb_viewMatrix;
	row_major float4x4	cb_viewMatrixInv;
	row_major float4x4	cb_projMatrix;
	row_major float4x4	cb_projMatrixInv;
	float3				cb_cameraPosition;
	uint				cb_numLights;
}

cbuffer cbWindowResize: register( b1 )
{
	uint cb_screenWidth;
	uint cb_screenHeight;

	int cb_padding0;
	int cb_padding1;
}

cbuffer cbObject : register( b2 )
{
	uint cb_numTriangles;

	int cb_padding2;
	int cb_padding3;
	int cb_padding4;
}

#endif //RAYTRACER_CONSTANT_BUFFERS_HLSL