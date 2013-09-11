#ifndef RAYTRACER_RAYS_H
#define RAYTRACER_RAYS_H

#include <vector>

#include <d3d11.h>
#include <DirectXMath.h>

struct RayDesc
{
	DirectX::XMFLOAT3	m_origin;
	float				m_maxDistance;
	DirectX::XMFLOAT3	m_direction;
	int					m_triangleId;
	DirectX::XMFLOAT3	m_reflectiveFactor;
	int					m_padding;

	RayDesc();
	RayDesc(
		DirectX::XMFLOAT3 p_origin,
		DirectX::XMFLOAT3 p_direction,
		DirectX::XMFLOAT3 p_reflectiveFactor,
		float p_maxDistance,
		int p_triangleId );
};

class Rays
{
public:
	Rays();
	~Rays();

	void csSetRayUAV(ID3D11DeviceContext* p_context, unsigned int p_startSlot);

	HRESULT init(ID3D11Device* p_device, unsigned int p_screenWidth, unsigned int p_screenHeight);
private:
	HRESULT initRayBuffer(ID3D11Device* p_device);
	HRESULT initRayUAV(ID3D11Device* p_device);

	std::vector<RayDesc>		m_rays;
	ID3D11Buffer*				m_rayBuffer;
	ID3D11UnorderedAccessView*	m_rayUAV;
};

#endif //RAYTRACER_RAYS_H