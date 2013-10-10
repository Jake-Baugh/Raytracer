#include "ManagementShader.h"
#include "Utility.h"

ManagementShader::ManagementShader()
{
	for( unsigned int i=0; i<CSIds_COUNT; i++ )
	{
		m_computeShaders[i] = nullptr;
		m_csBlobs[i]		= nullptr;
	}
}
ManagementShader::~ManagementShader()
{
	for( unsigned int i=0; i<CSIds_COUNT; i++ )
	{
		SAFE_RELEASE( m_computeShaders[i] );
		SAFE_RELEASE( m_csBlobs[i] );
	}
}

void ManagementShader::csSetShader( ID3D11DeviceContext* p_context, CSIds p_shaderId )
{
	if( p_shaderId < CSIds_COUNT )
		p_context->CSSetShader(m_computeShaders[p_shaderId], nullptr, 0);
}
void ManagementShader::csUnsetShader(ID3D11DeviceContext* p_context)
{
	p_context->CSSetShader(NULL, NULL, 0);
}

HRESULT ManagementShader::init( ID3D11Device* p_device )
{
	HRESULT hr = S_OK;
	hr = initCS(
			p_device,
			L"csPrimaryRayStage.cso",
			&m_computeShaders[CSIds_PRIMARY_RAY_STAGE], 
			&m_csBlobs[CSIds_PRIMARY_RAY_STAGE] );
	if(SUCCEEDED(hr))
	{
		hr = initCS(
				p_device,
				L"csIntersectionStage.cso",
				&m_computeShaders[CSIds_INTERSECTION_STAGE], 
				&m_csBlobs[CSIds_INTERSECTION_STAGE] );
	}
	if(SUCCEEDED(hr))
	{
		hr = initCS(
				p_device,
				L"csColorStage.cso",
				&m_computeShaders[CSIds_COLOR_STAGE], 
				&m_csBlobs[CSIds_COLOR_STAGE] );
	}
	return hr;
}
HRESULT ManagementShader::initCS(ID3D11Device* p_device,
								 std::wstring p_fileName,
								 ID3D11ComputeShader** p_shader,
								 ID3DBlob** p_blob)
{
	HRESULT hr = S_OK;

	hr = D3DReadFileToBlob(p_fileName.c_str(), p_blob);
	if(FAILED(hr))
	{
		MessageBox(
			0,
			L"ManagementShader::initCS() | D3DReadFileToBlob() | Failed",
			p_fileName.c_str(),
			MB_OK | MB_ICONEXCLAMATION);
	}
	else
	{
		hr = p_device->CreateComputeShader(
				(*p_blob)->GetBufferPointer(),
				(*p_blob)->GetBufferSize(),
				nullptr,
				p_shader);
		if(FAILED(hr))
		{
			MessageBox(
			0,
			L"ManagementShader::initCS() | CreateComputeShader | Failed",
			p_fileName.c_str(),
			MB_OK | MB_ICONEXCLAMATION);
		}
	}

	return hr;
}