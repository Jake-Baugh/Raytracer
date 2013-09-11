#ifndef RAYTRACER_VERTEX_H
#define RAYTRACER_VERTEX_H

#include <DirectXMath.h>

struct Vertex
{
	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT3 m_normal;
	DirectX::XMFLOAT2 m_texCoord;

	Vertex()
	{
		m_position	= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_normal	= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_texCoord	= DirectX::XMFLOAT2(0.0f, 0.0f);
	}
	Vertex(DirectX::XMFLOAT3 p_position, DirectX::XMFLOAT3 p_normal, DirectX::XMFLOAT2 p_texCoord)
	{
		m_position	= p_position;
		m_normal	= p_normal;
		p_texCoord	= p_texCoord;
	}
};

#endif //RAYTRACER_VERTEX_H