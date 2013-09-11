#include "Model.h"
#include "ObjLoader.h"
#include "Utility.h"

Model::Model()
{
	m_vertexBuffer	= nullptr;
	m_vertexSRV		= nullptr;
}
Model::~Model()
{
	SAFE_RELEASE(m_vertexBuffer);
	SAFE_RELEASE(m_vertexSRV);
}

void Model::csSetSRV(ID3D11DeviceContext* p_context, unsigned int p_startSlot)
{
	p_context->CSSetShaderResources(p_startSlot, 1, &m_vertexSRV);
}

unsigned int Model::getNumVertices()
{
	return m_vertices.size();
}

HRESULT Model::init(ID3D11Device* p_device, std::string p_fileName)
{
	HRESULT hr = S_OK;
	
	initVertices(p_fileName);

	hr = initVertexBuffer(p_device);
	if(SUCCEEDED(hr))
		hr = initVertexSRV(p_device);

	return hr;
}
void	Model::initVertices(std::string p_fileName)
{
	ObjLoader loader;
	loader.loadObj(p_fileName);
	m_vertices = loader.getLoadedVertices();
}
HRESULT Model::initVertexBuffer(ID3D11Device* p_device)
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.BindFlags			= D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.ByteWidth			= sizeof(Vertex) * m_vertices.size();
	bufferDesc.MiscFlags			= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride	= sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &m_vertices[0];

	hr = p_device->CreateBuffer(&bufferDesc, &initData, &m_vertexBuffer);
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
HRESULT Model::initVertexSRV(ID3D11Device* p_device)
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	m_vertexBuffer->GetDesc(&bufferDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));

	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.BufferEx.NumElements = bufferDesc.ByteWidth / bufferDesc.StructureByteStride;

	hr = p_device->CreateShaderResourceView(m_vertexBuffer, &srvDesc, &m_vertexSRV);
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