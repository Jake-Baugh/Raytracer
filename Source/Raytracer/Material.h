#ifndef RAYTRACER_MATERIAL_H
#define RAYTRACER_MATERIAL_H

#include <DirectXMath.h>

struct Material
{
	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;

	Material()
	{
		ambient = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		diffuse = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		specular = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	Material(DirectX::XMFLOAT3 p_ambient, DirectX::XMFLOAT3 p_diffuse, DirectX::XMFLOAT3 p_specular)
	{
		ambient  = DirectX::XMFLOAT4(p_ambient.x, p_ambient.y, p_ambient.z, 1.0f);
		diffuse  = DirectX::XMFLOAT4(p_diffuse.x, p_diffuse.y, p_diffuse.z, 1.0f);
		specular = DirectX::XMFLOAT4(p_specular.x, p_specular.y, p_specular.z, 0.0f);
	}
};

#endif //RAYTRACER_MATERIAL_H