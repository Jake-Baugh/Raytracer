#include "ManagementLight.h"
#include "Utility.h"

ManagementLight::ManagementLight()
{
	m_lightBuffer	= nullptr;
	m_lightSRV		= nullptr;
}
ManagementLight::~ManagementLight()
{
	SAFE_RELEASE(m_lightBuffer);
	SAFE_RELEASE(m_lightSRV);
}

void ManagementLight::csSetLightSRV(ID3D11DeviceContext* p_context, unsigned int p_startSlot)
{
	p_context->CSSetShaderResources(p_startSlot, 1, &m_lightSRV);
}

unsigned int ManagementLight::getNumLights()
{
	return m_lights.size();
}

HRESULT ManagementLight::init(ID3D11Device* p_device)
{
	HRESULT hr = S_OK;

	initLights();
	hr = initLightBuffer(p_device);
	if(SUCCEEDED(hr))
		hr = initLightSRV(p_device);

	return hr;
}
void ManagementLight::initLights()
{
	LightDesc light;
	light.m_position	= DirectX::XMFLOAT3(0.0f, -10.0f, 0.0f);
	light.m_ambient		= DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	light.m_diffuse		= DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	light.m_specular	= DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	light.m_attenuation	= DirectX::XMFLOAT3(0.0f, 0.03f, 0.0f);
	light.m_range		= 1000.0f;

	m_lights.push_back(light);

}
HRESULT ManagementLight::initLightBuffer(ID3D11Device* p_device)
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.BindFlags			= D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.ByteWidth			= sizeof(LightDesc) * m_lights.size();
	bufferDesc.MiscFlags			= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride	= sizeof(LightDesc);

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &m_lights[0];

	hr = p_device->CreateBuffer(&bufferDesc, &initData, &m_lightBuffer);
	if(FAILED(hr))
	{
		MessageBox(
			0,
			L"ManagementLight::initBuffer() | CreateBuffer() Failed",
			L"ManagementLight.h",
			MB_OK | MB_ICONEXCLAMATION);
	}

	return hr;
}
HRESULT ManagementLight::initLightSRV(ID3D11Device* p_device)
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	m_lightBuffer->GetDesc(&bufferDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));

	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.BufferEx.NumElements = bufferDesc.ByteWidth / bufferDesc.StructureByteStride;

	hr = p_device->CreateShaderResourceView(m_lightBuffer, &srvDesc, &m_lightSRV);
	if(FAILED(hr))
	{
		MessageBox(
			0,
			L"ManagementLight::initLightSRV() | CreateShaderResourceView() Failed",
			L"ManagementLight.h",
			MB_OK | MB_ICONEXCLAMATION);
	}

	return hr;
}