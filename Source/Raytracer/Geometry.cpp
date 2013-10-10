#include "Geometry.h"
#include "ObjLoader.h"
#include "Utility.h"

Geometry::Geometry()
{
	m_triangleBuffer = nullptr;
	m_triangleSRV	 = nullptr;
}
Geometry::~Geometry()
{
	SAFE_RELEASE(m_triangleBuffer);
	SAFE_RELEASE(m_triangleSRV);
}

void Geometry::csSetSRV(ID3D11DeviceContext* p_context, unsigned int p_startSlot)
{
	p_context->CSSetShaderResources(p_startSlot, 1, &m_triangleSRV);
}

unsigned int Geometry::getNumTriangles()
{
	return m_triangles.size();
}

HRESULT Geometry::init(ID3D11Device* p_device, std::string p_fileName)
{
	HRESULT hr = S_OK;
	
	initTriangles(p_fileName);

	hr = initTriangleBuffer(p_device);
	if(SUCCEEDED(hr))
		hr = initTriangleSRV(p_device);

	return hr;
}
void	Geometry::initTriangles(std::string p_fileName)
{
	ObjLoader loader;
	loader.loadObj(p_fileName);
	m_triangles = loader.getLoadedTriangles();
}
HRESULT Geometry::initTriangleBuffer(ID3D11Device* p_device)
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.BindFlags			= D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.ByteWidth			= sizeof(Triangle) * m_triangles.size();
	bufferDesc.MiscFlags			= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride	= sizeof(Triangle);

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &m_triangles[0];

	hr = p_device->CreateBuffer(&bufferDesc, &initData, &m_triangleBuffer);
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
HRESULT Geometry::initTriangleSRV(ID3D11Device* p_device)
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	m_triangleBuffer->GetDesc(&bufferDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));

	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.BufferEx.NumElements = bufferDesc.ByteWidth / bufferDesc.StructureByteStride;

	hr = p_device->CreateShaderResourceView(m_triangleBuffer, &srvDesc, &m_triangleSRV);
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