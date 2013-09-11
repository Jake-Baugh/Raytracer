#ifndef RAYTRACER_INTERSECTIONS_H
#define RAYTRACER_INTERSECTIONS_H

#include <vector>

#include <d3d11.h>
#include <DirectXMath.h>

struct IntersectDesc
{
	IntersectDesc();
	IntersectDesc(DirectX::XMFLOAT2 p_barCoords, float p_range, int p_triangleId);

	DirectX::XMFLOAT2 m_barCoords;
	
	float m_range;
	int	  m_triangleId;
};

class Intersections
{
public:
	Intersections();
	~Intersections();

	void csSetIntersectionUAV(ID3D11DeviceContext* p_context, unsigned int p_startSlot);

	HRESULT init(ID3D11Device* p_device, unsigned int p_screenWidth, unsigned int p_screenHeight);
private:
	HRESULT initIntersectionBuffer(ID3D11Device* p_device);
	HRESULT initIntersectionUAV(ID3D11Device* p_device);

	std::vector<IntersectDesc>	m_intersections;
	ID3D11Buffer*				m_intersectionBuffer;
	ID3D11UnorderedAccessView*	m_intersectionUAV;
};

#endif // RAYTRACER_INTERSECTIONS_H