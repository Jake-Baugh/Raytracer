#include "ManagementCB.h"
#include "Utility.h"

ManagementCB::ManagementCB()
{
	for(unsigned int i=0; i<CBIds::CBIds_COUNT; i++)
		m_constantBuffers[i] = nullptr;
}
ManagementCB::~ManagementCB()
{
	for(unsigned int i=0; i<CBIds::CBIds_COUNT; i++)
		SAFE_RELEASE(m_constantBuffers[i]);
}

void ManagementCB::csSetCB(ID3D11DeviceContext* p_context, CBIds::Id p_cbId)
{
	if(p_cbId < CBIds::CBIds_COUNT)
		p_context->CSSetConstantBuffers(p_cbId, 1, &m_constantBuffers[p_cbId]);
}

void ManagementCB::updateCBFrame(ID3D11DeviceContext* p_context,
								 DirectX::XMFLOAT4X4  p_viewMatrix,
								 DirectX::XMFLOAT4X4  p_viewMatrixInv,
								 DirectX::XMFLOAT4X4  p_projMatrix,
								 DirectX::XMFLOAT4X4  p_projMatrixInv,
								 DirectX::XMFLOAT3	  p_cameraPosition,
								 unsigned int		  p_numLights )
{
	CBFrame cBuffer;
	cBuffer.m_viewMatrix	 = p_viewMatrix;
	cBuffer.m_viewMatrixInv  = p_viewMatrixInv;
	cBuffer.m_projMatrix	 = p_projMatrix;
	cBuffer.m_projMatrixInv  = p_projMatrixInv;
	cBuffer.m_cameraPosition = p_cameraPosition;
	cBuffer.m_numLights		 = p_numLights;

	p_context->UpdateSubresource(m_constantBuffers[CBIds::CBIds_FRAME], 0, 0, &cBuffer, 0, 0);
}
void ManagementCB::updateCBWindowResize(ID3D11DeviceContext* p_context,
										unsigned int p_screenWidth,
										unsigned int p_screenHeight)
{
	CBWindowResize cBuffer;
	cBuffer.m_screenWidth = p_screenWidth;
	cBuffer.m_screenHeight = p_screenHeight;

	p_context->UpdateSubresource(m_constantBuffers[CBIds::CBIds_WINDOW_RESIZE], 0, 0, &cBuffer, 0, 0);
}
void ManagementCB::updateCBObject(ID3D11DeviceContext* p_context, unsigned int p_numVertices)
{
	CBObject cBuffer;
	cBuffer.m_numTriangles = p_numVertices;

	p_context->UpdateSubresource(m_constantBuffers[CBIds::CBIds_OBJECT], 0, 0, &cBuffer, 0, 0);
}

HRESULT ManagementCB::init(ID3D11Device* p_device)
{
	HRESULT hr = S_OK;

	hr = initCB(p_device, &m_constantBuffers[CBIds::CBIds_FRAME], sizeof(CBFrame));
	if(SUCCEEDED(hr))
		hr = initCB(p_device, &m_constantBuffers[CBIds::CBIds_WINDOW_RESIZE], sizeof(CBWindowResize));
	if(SUCCEEDED(hr))
		hr = initCB(p_device, &m_constantBuffers[CBIds::CBIds_OBJECT], sizeof(CBObject));

	return hr;
}
HRESULT ManagementCB::initCB(ID3D11Device* p_device, ID3D11Buffer** p_constantBuffer, unsigned int p_size)
{
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	
	bufferDesc.Usage			= D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth		= p_size;
	bufferDesc.BindFlags		= D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags	= 0;
	
	hr = p_device->CreateBuffer(&bufferDesc, NULL, p_constantBuffer);
	if(FAILED(hr))
	{
		MessageBox(
			NULL,
			L"ManagmenetCB::initConstantBuffer() | CreateBuffer() | Failed",
			L"ConstantBuffer fail",
			MB_OK | MB_ICONEXCLAMATION); 
	}
	return hr;
}