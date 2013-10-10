#include "ManagementTex.h"
#include "Utility.h"

#include <DDSTextureLoader.h>

ManagementTex::ManagementTex()
{
	for(unsigned int i=0; i<TexIds::TexIds_COUNT; i++)
		m_srvTextures[i] = nullptr;
}
ManagementTex::~ManagementTex()
{
	for(unsigned int i=0; i<TexIds::TexIds_COUNT; i++)
		SAFE_RELEASE(m_srvTextures[i]);
}

void ManagementTex::csSetTexture(ID3D11DeviceContext* p_context, TexIds::Id p_texId, unsigned int p_startSlot)
{
	if(p_texId >= 0 && p_texId < TexIds::TexIds_COUNT)
		p_context->CSSetShaderResources(p_startSlot, 1, &m_srvTextures[p_texId]);
}

HRESULT ManagementTex::init(ID3D11Device* p_device)
{
	HRESULT hr = S_OK;

	hr = loadTexture(p_device, L"../Resources/boxTex.dds", TexIds::TexIds_CUBE);

	return hr;
}
HRESULT ManagementTex::loadTexture(ID3D11Device* p_device, std::wstring p_fileName, TexIds::Id p_texId)
{
	HRESULT hr = S_OK;
	ID3D11Resource* texture = NULL;
	ID3D11ShaderResourceView* srvTexture = NULL;

	hr = DirectX::CreateDDSTextureFromFile( p_device, p_fileName.c_str(),
		&texture, &srvTexture );
	if(FAILED(hr))
		MessageBox(NULL,
		L"ManagmenetTex::loadTexture() | CreateDDSTextureFromFile() | Failed",
		p_fileName.c_str(), MB_OK | MB_ICONEXCLAMATION);

	m_srvTextures[p_texId] = srvTexture;
	SAFE_RELEASE(texture);

	return hr;
}