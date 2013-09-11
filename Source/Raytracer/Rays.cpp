#include "Rays.h"
#include "Utility.h"

RayDesc::RayDesc()
{
	m_origin			= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_maxDistance		= 0.0f;
	m_direction			= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_triangleId		= -1;
	m_reflectiveFactor	= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
}
RayDesc::RayDesc(DirectX::XMFLOAT3 p_origin,
				 DirectX::XMFLOAT3 p_direction,
				 DirectX::XMFLOAT3 p_reflectiveFactor,
				 float p_maxDistance,
				 int p_triangleId )
{
	m_origin			= p_origin;
	m_direction			= p_direction;
	m_reflectiveFactor	= p_reflectiveFactor;
	m_maxDistance		= p_maxDistance;
	m_triangleId		= p_triangleId;
}


Rays::Rays()
{
	m_rayBuffer = nullptr;
	m_rayUAV	= nullptr;
}
Rays::~Rays()
{
	SAFE_RELEASE(m_rayBuffer);
	SAFE_RELEASE(m_rayUAV);
}

void Rays::csSetRayUAV(ID3D11DeviceContext* p_context, unsigned int p_startSlot)
{
	p_context->CSSetUnorderedAccessViews(p_startSlot, 1, &m_rayUAV, nullptr);
}

HRESULT Rays::init(ID3D11Device* p_device, unsigned int p_screenWidth, unsigned int p_screenHeight)
{
	HRESULT hr = S_OK;

	m_rays.resize(p_screenWidth*p_screenHeight);

	hr = initRayBuffer(p_device);
	if(SUCCEEDED(hr))
		hr = initRayUAV(p_device);

	return hr;
}
HRESULT Rays::initRayBuffer(ID3D11Device* p_device)
{
	HRESULT hr = S_OK;
	
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.BindFlags			= D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.ByteWidth			= sizeof(RayDesc) * m_rays.size();
	bufferDesc.MiscFlags			= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride  = sizeof(RayDesc);

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &m_rays[0];

	hr = p_device->CreateBuffer(&bufferDesc, &initData, &m_rayBuffer);
	if(FAILED(hr))
	{
		MessageBox(
		0,
		L"Rays::initRayBuffer() | Failed",
		L"Rays",
		MB_OK | MB_ICONEXCLAMATION);
	}
	return hr;
}
HRESULT Rays::initRayUAV(ID3D11Device* p_device)
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	m_rayBuffer->GetDesc(&bufferDesc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));

	uavDesc.ViewDimension		= D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Format				= DXGI_FORMAT_UNKNOWN;
	uavDesc.Buffer.NumElements	= bufferDesc.ByteWidth / bufferDesc.StructureByteStride;

	hr = p_device->CreateUnorderedAccessView(m_rayBuffer, &uavDesc, &m_rayUAV);
	if(FAILED(hr))
	{
		MessageBox(
		0,
		L"Rays::initRayUAV() | Failed",
		L"Rays",
		MB_OK | MB_ICONEXCLAMATION);
	}

	return hr;
}