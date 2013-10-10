#include "ManagementMaterial.h"
#include "Utility.h"

ManagementMaterial::ManagementMaterial()
{
	m_materialBuffer = NULL;
	m_materialSRV	 = NULL;
}
ManagementMaterial::~ManagementMaterial()
{
	SAFE_RELEASE(m_materialBuffer);
	SAFE_RELEASE(m_materialSRV);
}

void ManagementMaterial::csSetSRV(ID3D11DeviceContext* p_context, unsigned int p_startSlot)
{
	p_context->CSSetShaderResources(p_startSlot, 1, &m_materialSRV);
}

HRESULT ManagementMaterial::init(ID3D11Device* p_device, std::vector<Material> p_materials)
{
	HRESULT hr = S_OK;

	m_materials = p_materials;

	hr = initMaterialBuffer(p_device);
	if(SUCCEEDED(hr))
		hr = initMaterialSRV(p_device);

	return hr;
}

HRESULT ManagementMaterial::initMaterialBuffer(ID3D11Device* p_device)
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.BindFlags			= D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.ByteWidth			= sizeof(Material) * m_materials.size();
	bufferDesc.MiscFlags			= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride	= sizeof(Material);

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &m_materials[0];

	hr = p_device->CreateBuffer(&bufferDesc, &initData, &m_materialBuffer);
	if(FAILED(hr))
	{
		MessageBox(
			0,
			L"Model::initVertexBuffer | CreateBuffer() Failed",
			L"Model.h",
			MB_OK | MB_ICONEXCLAMATION);
	}

	return hr;
}
HRESULT ManagementMaterial::initMaterialSRV(ID3D11Device* p_device)
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	m_materialBuffer->GetDesc(&bufferDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));

	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.BufferEx.NumElements = bufferDesc.ByteWidth / bufferDesc.StructureByteStride;

	hr = p_device->CreateShaderResourceView(m_materialBuffer, &srvDesc, &m_materialSRV);
	if(FAILED(hr))
	{
		MessageBox(
			0,
			L"Model::initVertexSRV | CreateShaderResourceView() Failed",
			L"Model.h",
			MB_OK | MB_ICONEXCLAMATION);
	}

	return hr;
}