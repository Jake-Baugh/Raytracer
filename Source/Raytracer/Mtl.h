#ifndef RAYTRACER_MTL_H
#define RAYTRACER_MTL_H

#include <DirectXMath.h>
#include <string>

struct Mtl
{
	std::wstring name;

	DirectX::XMFLOAT3 ambient;
	DirectX::XMFLOAT3 diffuse;
	DirectX::XMFLOAT3 specular;
	
	float specularCoefficient;
	float transparency;
	float indexOfRefraction;

	std::wstring ambientTexName;
	std::wstring diffuseTexName;
	std::wstring specularTexName;
	std::wstring specularHighlightTexName;
	std::wstring alphaTexName;
	std::wstring bumpMapName;
	std::wstring displacementMapName;
	std::wstring stencilDecalMapName;

	Mtl()
	{
		name = L"Unknown";

		ambient  = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		diffuse  = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		specular = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

		specularCoefficient = 0.0f;
		transparency		= 0.0f;
		indexOfRefraction	= 0.0f;
	
		ambientTexName			 = L"Unknown";
		diffuseTexName			 = L"Unknown";
		specularTexName			 = L"Unknown";
		specularHighlightTexName = L"Unknown";
		alphaTexName			 = L"Unknown";
		bumpMapName				 = L"Unknown";
		displacementMapName		 = L"Unknown";
		stencilDecalMapName		 = L"Unknown";
	}
	
};		

#endif // RAYTRACER_MTL_H