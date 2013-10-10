#ifndef RAYTRACER_CB_DESC_H
#define RAYTRACER_CB_DESC_H

#include <DirectXMath.h>

struct CBIds
{
	enum Id
	{
		CBIds_FRAME,
		CBIds_WINDOW_RESIZE,
		CBIds_OBJECT,
		CBIds_COUNT
	};
};

struct CBFrame
{
	DirectX::XMFLOAT4X4 m_viewMatrix;
	DirectX::XMFLOAT4X4 m_viewMatrixInv;
	DirectX::XMFLOAT4X4 m_projMatrix;
	DirectX::XMFLOAT4X4 m_projMatrixInv;
	DirectX::XMFLOAT3	m_cameraPosition;
	unsigned int		m_numLights;
};

struct CBWindowResize
{
	unsigned int m_screenWidth;
	unsigned int m_screenHeight;

	int m_padding0;
	int m_padding1;
};

struct CBObject
{
	unsigned int m_numTriangles;
	
	int m_padding2;
	int m_padding3;
	int m_padding4;
};

#endif //RAYTRACER_CB_DESC_H