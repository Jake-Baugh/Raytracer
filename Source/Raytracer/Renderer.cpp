#include "Renderer.h"

#include "ManagementD3D.h"
#include "ManagementShader.h"
#include "ManagementCB.h"
#include "ManagementLight.h"
#include "ManagementTex.h"
#include "ManagementSS.h"
#include "Intersections.h"
#include "Rays.h"
#include "Utility.h"
#include "ObjLoader.h"
#include "Model.h"

Renderer::Renderer()
{
	m_managementD3D		= nullptr;
	m_managementShader	= nullptr;
	m_managementCB		= nullptr;
	m_managementLight	= nullptr;
	m_managementTex		= nullptr;
	m_managementSS		= nullptr;
	m_intersections		= nullptr;
	m_rays				= nullptr;
	m_cube				= nullptr;
}
Renderer::~Renderer()
{
	SAFE_DELETE( m_managementD3D );
	SAFE_DELETE( m_managementShader );
	SAFE_DELETE( m_managementCB );
	SAFE_DELETE( m_managementLight );
	SAFE_DELETE( m_managementTex );
	SAFE_DELETE( m_managementSS );
	SAFE_DELETE( m_intersections );
	SAFE_DELETE( m_rays );
	SAFE_DELETE( m_cube );
}

void Renderer::render(DirectX::XMFLOAT4X4 p_viewMatrix,
					  DirectX::XMFLOAT4X4 p_viewMatrixInv,
					  DirectX::XMFLOAT4X4 p_projMatrix,
					  DirectX::XMFLOAT4X4 p_projMatrixInv,
					  DirectX::XMFLOAT3	  p_cameraPosition)
{
	ID3D11DeviceContext* context = m_managementD3D->getDeviceContext();	
	m_managementCB->csSetCB(context, CBIds::CBIds_FRAME);
	m_managementCB->updateCBFrame(
		context,
		p_viewMatrix,
		p_viewMatrixInv,
		p_projMatrix,
		p_projMatrixInv,
		p_cameraPosition,
		m_managementLight->getNumLights());

	primaryRayStage();
	
	unsigned int rayBounces = 2;
	for(unsigned int i=0; i<rayBounces; i++)
	{
		intersectionStage();
		colorStage();
	}

	m_managementD3D->present();
}

HRESULT Renderer::init(  HWND p_windowHandle, unsigned int p_screenWidth, unsigned int p_screenHeight )
{
	HRESULT hr = S_OK;

	hr = initManagementD3D( p_windowHandle, p_screenWidth, p_screenHeight );
	if(SUCCEEDED(hr))
		hr = initManagementShader(m_managementD3D->getDevice());
	if(SUCCEEDED(hr))
		hr = initManagementCB(m_managementD3D->getDevice());
	if(SUCCEEDED(hr))
		hr = initManagementLight(m_managementD3D->getDevice());
	if(SUCCEEDED(hr))
		hr = initManagementTex(m_managementD3D->getDevice());
	if(SUCCEEDED(hr))
		hr = initManagementSS(m_managementD3D->getDevice());
	if(SUCCEEDED(hr))
		hr = initIntersections(m_managementD3D->getDevice(), p_screenWidth, p_screenHeight);
	if(SUCCEEDED(hr))
		hr = initRays(m_managementD3D->getDevice(), p_screenWidth, p_screenHeight);
	if(SUCCEEDED(hr))
		hr = initCube(m_managementD3D->getDevice());

	if(SUCCEEDED(hr))
	{
		m_managementCB->csSetCB(m_managementD3D->getDeviceContext(), CBIds::CBIds_WINDOW_RESIZE);
		m_managementCB->updateCBWindowResize(m_managementD3D->getDeviceContext(), p_screenWidth, p_screenHeight);
	}

	return hr;
}
HRESULT Renderer::initManagementD3D(  HWND p_windowHandle, unsigned int p_screenWidth, unsigned int p_screenHeight )
{
	HRESULT hr = S_OK;
	m_managementD3D = new ManagementD3D();
	m_managementD3D->init( p_windowHandle, p_screenWidth, p_screenHeight );
	return hr;
}
HRESULT Renderer::initManagementShader( ID3D11Device* p_device )
{
	HRESULT hr = S_OK;
	m_managementShader = new ManagementShader();
	m_managementShader->init(p_device);
	return hr;
}
HRESULT Renderer::initManagementCB( ID3D11Device* p_device )
{
	HRESULT hr = S_OK;
	m_managementCB = new ManagementCB();
	m_managementCB->init( p_device );
	return hr;
}
HRESULT Renderer::initManagementLight( ID3D11Device* p_device)
{
	HRESULT hr = S_OK;
	m_managementLight = new ManagementLight();
	m_managementLight->init(p_device);
	return hr;
}
HRESULT Renderer::initManagementTex(ID3D11Device* p_device)
{
	HRESULT hr = S_OK;
	m_managementTex = new ManagementTex();
	m_managementTex->init(p_device);
	return hr;
}
HRESULT Renderer::initManagementSS(ID3D11Device* p_device)
{
	HRESULT hr = S_OK;
	m_managementSS = new ManagementSS();
	m_managementSS->init(p_device);
	return hr;
}
HRESULT Renderer::initIntersections(ID3D11Device* p_device, unsigned int p_screenWidth, unsigned int p_screenHeight)
{
	HRESULT hr = S_OK;
	m_intersections = new Intersections();
	m_intersections->init(p_device, p_screenWidth, p_screenHeight);
	return hr;
}
HRESULT Renderer::initRays(ID3D11Device* p_device, unsigned int p_screenWidth, unsigned int p_screenHeight)
{
	HRESULT hr = S_OK;
	m_rays = new Rays();
	m_rays->init(p_device, p_screenWidth, p_screenHeight);
	return hr;
}
HRESULT Renderer::initCube(ID3D11Device* p_device)
{
	HRESULT hr = S_OK;
	m_cube = new Model();
	hr = m_cube->init(p_device, "Resources/box.obj");
	return hr;
}

void Renderer::primaryRayStage()
{
	ID3D11DeviceContext* context = m_managementD3D->getDeviceContext();
	m_managementShader->csSetShader(context, ManagementShader::CSIds_PRIMARY_RAY_STAGE);
	m_rays->csSetRayUAV(context, 0);
	m_managementD3D->setAccumulationUAV(1);

	context->Dispatch(25, 25, 1);
}
void Renderer::intersectionStage()
{
	ID3D11DeviceContext* context = m_managementD3D->getDeviceContext();
	m_managementShader->csSetShader(context, ManagementShader::CSIds_INTERSECTION_STAGE);
	m_rays->csSetRayUAV(context, 0);
	m_intersections->csSetIntersectionUAV(context, 1);
	m_cube->csSetSRV(context, 0);

	m_managementCB->csSetCB(context, CBIds::CBIds_OBJECT);
	m_managementCB->updateCBObject(context, m_cube->getNumVertices());

	context->Dispatch(25, 25, 1);
}
void Renderer::colorStage()
{
	ID3D11DeviceContext* context = m_managementD3D->getDeviceContext();
	m_managementShader->csSetShader(context, ManagementShader::CSIds_COLOR_STAGE);
	m_managementD3D->setBackBuffer();
	m_managementD3D->setAccumulationUAV(2);
	m_rays->csSetRayUAV(context, 3);
	m_intersections->csSetIntersectionUAV(context, 1);
	m_cube->csSetSRV(context, 0);
	m_managementLight->csSetLightSRV(context, 1);
	m_managementTex->csSetTexture(context, TexIds::TexIds_CUBE, 2);
	m_managementSS->csSetSS(context, ManagementSS::SSTypes_DEFAULT, 0);
	
	context->Dispatch(25, 25, 1);
}