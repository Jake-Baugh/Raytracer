#ifndef RAYTRACER_LIGHT_DESC_H
#define RAYTRACER_LIGHT_DESC_H

#include <DirectXMath.h>

struct LightDesc
{
	DirectX::XMFLOAT3 m_position;
	float			  m_spotPower;
	DirectX::XMFLOAT3 m_direction;
	float			  m_range;
	DirectX::XMFLOAT4 m_ambient;
	DirectX::XMFLOAT4 m_diffuse;
	DirectX::XMFLOAT4 m_specular;
	DirectX::XMFLOAT3 m_attenuation;
	int				  m_lightType;

	LightDesc()
	{
		m_position	= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_spotPower = 0.0f;
		m_direction = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_range		= 0.0f;
		m_ambient	= DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		m_diffuse	= DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		m_specular	= DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		m_lightType = -1;
	}
};

#endif //RAYTRACER_LIGHT_DESC_H