#include "Intersections.h"
#include "Utility.h"

IntersectDesc::IntersectDesc()
{
	m_barCoords	 = DirectX::XMFLOAT2(0.0f, 0.0f);
	m_range		 = 0.0f;
	m_triangleId = -1;
}
IntersectDesc::IntersectDesc(DirectX::XMFLOAT2 p_barCoords, float p_range, int p_triangleId)
{
	m_barCoords  = p_barCoords;
	m_range		 = p_range;
	m_triangleId = p_triangleId;
}

Intersections::Intersections()
{
	m_intersectionBuffer = nullptr;
	m_intersectionUAV	 = nullptr;
}
Intersections::~Intersections()
{
	SAFE_RELEASE(m_intersectionBuffer);
	SAFE_RELEASE(m_intersectionUAV);
}

void Intersections::csSetIntersectionUAV(ID3D11DeviceContext* p_context, unsigned int p_startSlot)
{
	p_context->CSSetUnorderedAccessViews(p_startSlot, 1, &m_intersectionUAV, nullptr);
}

HRESULT Intersections::init(ID3D11Device* p_device, unsigned int p_screenWidth, unsigned int p_screenHeight)
{
	HRESULT hr = S_OK;

	m_intersections.resize(p_screenWidth * p_screenHeight);

	hr = initIntersectionBuffer(p_device);
	if(SUCCEEDED(hr))
		hr = initIntersectionUAV(p_device);

	return hr;
}
HRESULT Intersections::initIntersectionBuffer(ID3D11Device* p_device)
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.BindFlags		   = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.ByteWidth		   = sizeof(IntersectDesc) * m_intersections.size();
	bufferDesc.MiscFlags		   = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = sizeof(IntersectDesc);

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &m_intersections[0];

	hr = p_device->CreateBuffer(&bufferDesc, &initData, &m_intersectionBuffer);
	if(FAILED(hr))
	{
		MessageBox(
			0,
			L"Intersections::initIntersectionBuffer() | Failed",
			L"Intersections",
			MB_OK | MB_ICONEXCLAMATION);
	}

	return hr;
}
HRESULT Intersections::initIntersectionUAV(ID3D11Device* p_device)
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	m_intersectionBuffer->GetDesc(&bufferDesc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));

	uavDesc.ViewDimension		= D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Format				= DXGI_FORMAT_UNKNOWN;
	uavDesc.Buffer.NumElements	= bufferDesc.ByteWidth / bufferDesc.StructureByteStride;

	hr = p_device->CreateUnorderedAccessView(m_intersectionBuffer, &uavDesc, &m_intersectionUAV);
	if(FAILED(hr))
	{
		MessageBox(
			0,
			L"Intersections::initIntersectionUAV() | Failed",
			L"Intersections",
			MB_OK | MB_ICONEXCLAMATION);
	}

	return hr;
}