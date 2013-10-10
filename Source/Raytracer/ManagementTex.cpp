#include "ManagementTex.h"
#include "Utility.h"

#include <DDSTextureLoader.h>

ManagementTex::ManagementTex()
{
	m_textureArray = NULL;
	m_textureArraySRV = NULL;
}
ManagementTex::~ManagementTex()
{
	SAFE_RELEASE(m_textureArray);
	SAFE_RELEASE(m_textureArraySRV);
}

void ManagementTex::csSetTexture(ID3D11DeviceContext* p_context, unsigned int p_startSlot)
{
	p_context->CSSetShaderResources(p_startSlot, 1, &m_textureArraySRV);
}

HRESULT ManagementTex::init(ID3D11Device* p_device, ID3D11DeviceContext* p_context, std::vector<std::wstring> p_texFilenames)
{
	HRESULT hr = S_OK;
	hr = initTextureArray(p_device, p_texFilenames.size());
	for(unsigned int i=0; i<p_texFilenames.size(); i++)
	{
		if(SUCCEEDED(hr))
			hr = loadTexture(p_device, p_context, p_texFilenames[i], i);
	}

	return hr;
}

HRESULT ManagementTex::initTextureArray(ID3D11Device* p_device, unsigned int p_texCnt)
{
	HRESULT hr = S_OK;

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width				= 1024;
	texDesc.Height				= 1024;
	texDesc.MipLevels			= 1;
	texDesc.ArraySize			= p_texCnt;
	texDesc.SampleDesc.Count	= 1;
	texDesc.SampleDesc.Quality	= 0;
	texDesc.Format				= DXGI_FORMAT_B8G8R8X8_UNORM;
	texDesc.Usage				= D3D11_USAGE_DEFAULT;
	texDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags		= 0;
	texDesc.MiscFlags			= 0;
	p_device->CreateTexture2D(&texDesc, NULL, &m_textureArray);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.ViewDimension =  D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Format = DXGI_FORMAT_B8G8R8X8_UNORM;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.ArraySize = p_texCnt;
	p_device->CreateShaderResourceView(m_textureArray, &srvDesc, &m_textureArraySRV);

	return hr;
}

HRESULT ManagementTex::loadTexture(ID3D11Device* p_device,
								   ID3D11DeviceContext* p_context,
								   std::wstring p_fileName,
								   unsigned int p_arraySlice)
{
	HRESULT hr = S_OK;
	ID3D11Resource* texture = NULL;
	ID3D11ShaderResourceView* srvTexture = NULL;

	p_fileName = L"../Resources/" + p_fileName;

	hr = DirectX::CreateDDSTextureFromFile( p_device, p_fileName.c_str(),
		&texture, NULL, 1 );
	if(FAILED(hr))
		MessageBox(NULL,
		L"ManagmenetTex::loadTexture() | CreateDDSTextureFromFile() | Failed",
		p_fileName.c_str(), MB_OK | MB_ICONEXCLAMATION);

	p_context->CopySubresourceRegion(m_textureArray, D3D11CalcSubresource(0, p_arraySlice, 1), 0, 0, 0, texture, 0, NULL); 

	return hr;
}