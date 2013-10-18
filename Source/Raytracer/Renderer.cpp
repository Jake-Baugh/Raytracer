#include "Renderer.h"

#include <string>

#include "ManagementD3D.h"
#include "ManagementShader.h"
#include "ManagementCB.h"
#include "ManagementLight.h"
#include "ManagementTex.h"
#include "ManagementSS.h"
#include "ManagementMaterial.h"
#include "Intersections.h"
#include "Rays.h"
#include "Utility.h"
#include "ObjLoader.h"
#include "Geometry.h"
#include "Material.h"
#include "Octree.h"
#include "Timer.h"

Renderer::Renderer()
{
	m_managementD3D		 = nullptr;
	m_managementShader	 = nullptr;
	m_managementCB		 = nullptr;
	m_managementLight	 = nullptr;
	m_managementTex		 = nullptr;
	m_managementSS		 = nullptr;
	m_managementMaterial = nullptr;
	m_intersections		 = nullptr;
	m_rays				 = nullptr;
	m_geometry			 = nullptr;
	m_octree			 = nullptr;
}
Renderer::~Renderer()
{
	SAFE_DELETE( m_managementD3D );
	SAFE_DELETE( m_managementShader );
	SAFE_DELETE( m_managementCB );
	SAFE_DELETE( m_managementLight );
	SAFE_DELETE( m_managementTex );
	SAFE_DELETE( m_managementSS );
	SAFE_DELETE( m_managementMaterial );
	SAFE_DELETE( m_intersections );
	SAFE_DELETE( m_rays );
	SAFE_DELETE( m_geometry );
	SAFE_DELETE( m_octree );
	
	for(unsigned int i=0; i<m_timers.size(); i++)
		SAFE_DELETE( m_timers[i] );
}

std::vector<Timer*> Renderer::getTimers()
{
	return m_timers;
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

	m_threadCountX = p_screenWidth / BLOCK_SIZE_X;
	if(p_screenWidth % BLOCK_SIZE_X != 0)
		m_threadCountX += 1;
	m_threadCountY = p_screenHeight / BLOCK_SIZE_Y;
	if(p_screenHeight % BLOCK_SIZE_Y != 0)
		m_threadCountY += 1;

	hr = initManagementD3D( p_windowHandle, p_screenWidth, p_screenHeight );
	if(SUCCEEDED(hr))
		hr = initManagementShader(m_managementD3D->getDevice());
	if(SUCCEEDED(hr))
		hr = initManagementCB(m_managementD3D->getDevice());
	if(SUCCEEDED(hr))
		hr = initManagementLight(m_managementD3D->getDevice());
	if(SUCCEEDED(hr))
		hr = initManagementSS(m_managementD3D->getDevice());
	if(SUCCEEDED(hr))
		hr = initIntersections(m_managementD3D->getDevice(), p_screenWidth, p_screenHeight);
	if(SUCCEEDED(hr))
		hr = initRays(m_managementD3D->getDevice(), p_screenWidth, p_screenHeight);

	if(SUCCEEDED(hr))
		hr = loadObj(m_managementD3D->getDevice(), m_managementD3D->getDeviceContext());

	if(SUCCEEDED(hr))
	{
		m_managementCB->csSetCB(m_managementD3D->getDeviceContext(), CBIds::CBIds_WINDOW_RESIZE);
		m_managementCB->updateCBWindowResize(m_managementD3D->getDeviceContext(), p_screenWidth, p_screenHeight);
	}
	if(SUCCEEDED(hr))
		hr = initTimer(m_managementD3D->getDevice());

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
HRESULT Renderer::initManagementTex(ID3D11Device* p_device, ID3D11DeviceContext* p_context, std::vector<std::wstring> p_texFilenames)
{
	HRESULT hr = S_OK;
	m_managementTex = new ManagementTex();
	m_managementTex->init(p_device, p_context, p_texFilenames);
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
HRESULT Renderer::initGeometry(ID3D11Device* p_device, std::vector<Triangle> p_triangles)
{
	HRESULT hr = S_OK;
	m_geometry = new Geometry();
	hr = m_geometry->init(p_device, p_triangles);
	return hr;
}
HRESULT Renderer::initManagementMaterial(ID3D11Device* p_device, std::vector<Material> p_materials)
{
	HRESULT hr = S_OK;
	m_managementMaterial = new ManagementMaterial();
	hr = m_managementMaterial->init(p_device, p_materials);
	return hr;
}

HRESULT Renderer::loadObj(ID3D11Device* p_device, ID3D11DeviceContext* p_context)
{
	HRESULT hr = S_OK;

	ObjLoader objLoader;
	objLoader.loadObj("../Resources/box3.obj");

	std::vector<Triangle> triangles = objLoader.getLoadedTriangles();
	std::vector<Mtl> mtls = objLoader.getLoadedMtls();
	std::vector<Material> materials;
	std::vector<std::wstring> textureNames;
	for(unsigned int i=0; i<mtls.size(); i++)
	{
		materials.push_back(Material(mtls[i].ambient, mtls[i].diffuse, mtls[i].specular));
		textureNames.push_back(mtls[i].diffuseTexName);
	}
	if(materials.size() == 0)
		materials.push_back(Material());

	hr = initGeometry(p_device, triangles);
	if(SUCCEEDED(hr))
		hr = initManagementMaterial(p_device, materials);
	if(SUCCEEDED(hr))
		hr = initManagementTex(p_device, p_context, textureNames);
	if(SUCCEEDED(hr))
		hr = initOctree(p_device, triangles);

	return hr;
}

HRESULT Renderer::initOctree(ID3D11Device* p_device, std::vector<Triangle> p_triangles)
{
	HRESULT hr = S_OK;
	m_octree = new Octree();
	hr = m_octree->init(p_device, p_triangles);
	return hr;
}

HRESULT Renderer::initTimer(ID3D11Device* p_device)
{
	HRESULT hr = S_OK;
	for(unsigned int i=0; i<3; i++)
	{
		m_timers.push_back(new Timer());
		if(SUCCEEDED(hr))
			hr = m_timers[i]->init(p_device);
	}
	return hr;
}

void Renderer::primaryRayStage()
{
	ID3D11DeviceContext* context = m_managementD3D->getDeviceContext();
	m_managementShader->csSetShader(context, ManagementShader::CSIds_PRIMARY_RAY_STAGE);
	m_rays->csSetRayUAV(context, 0);
	m_managementD3D->setAccumulationUAV(1);

//	m_timers[0]->start(context);
	context->Dispatch(m_threadCountX, m_threadCountY, 1);
//	m_timers[0]->stop(context);
//	m_timers[0]->getTime(context);

	ID3D11UnorderedAccessView* uav = nullptr;
	context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
	context->CSSetUnorderedAccessViews(1, 1, &uav, nullptr);
	m_managementShader->csUnsetShader(context);
}
void Renderer::intersectionStage()
{
	ID3D11DeviceContext* context = m_managementD3D->getDeviceContext();
	m_managementShader->csSetShader(context, ManagementShader::CSIds_INTERSECTION_STAGE);
	m_rays->csSetRayUAV(context, 0);
	m_intersections->csSetIntersectionUAV(context, 1);
	m_geometry->csSetSRV(context, 0);
	m_octree->csSetNodeSRV(context, 1);

	m_managementCB->csSetCB(context, CBIds::CBIds_OBJECT);
	m_managementCB->updateCBObject(context, m_geometry->getNumTriangles());

	m_timers[1]->start(context);
	context->Dispatch(m_threadCountX, m_threadCountY, 1);
	m_timers[1]->stop(context);
	m_timers[1]->getTime(context);

	ID3D11UnorderedAccessView* uav = nullptr;
	context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
	context->CSSetUnorderedAccessViews(1, 1, &uav, nullptr);
	m_managementShader->csUnsetShader(context);
}
void Renderer::colorStage()
{
	ID3D11DeviceContext* context = m_managementD3D->getDeviceContext();
	m_managementShader->csSetShader(context, ManagementShader::CSIds_COLOR_STAGE);
	m_managementD3D->setBackBuffer();
	m_managementD3D->setAccumulationUAV(2);
	m_rays->csSetRayUAV(context, 3);
	m_intersections->csSetIntersectionUAV(context, 1);
	m_geometry->csSetSRV(context, 0);
	m_managementLight->csSetLightSRV(context, 1);
	m_managementTex->csSetTexture(context, 2);
	m_managementMaterial->csSetSRV(context, 3);
	m_octree->csSetNodeSRV(context, 4);
	m_managementSS->csSetSS(context, ManagementSS::SSTypes_DEFAULT, 0);
	
//	m_timers[2]->start(context);
	context->Dispatch(m_threadCountX, m_threadCountY, 1);
//	m_timers[2]->stop(context);
//	m_timers[2]->getTime(context);

	ID3D11UnorderedAccessView* uav = nullptr;
	context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
	context->CSSetUnorderedAccessViews(1, 1, &uav, nullptr);
	context->CSSetUnorderedAccessViews(2, 1, &uav, nullptr);
	context->CSSetUnorderedAccessViews(3, 1, &uav, nullptr);
	m_managementShader->csUnsetShader(context);
}