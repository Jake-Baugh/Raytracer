#ifndef RAYTRACER_RENDERER_H
#define RAYTRACER_RENDERER_H

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

#include "Triangle.h"
#include "Material.h"

class ManagementD3D;
class ManagementShader;
class ManagementCB;
class ManagementLight;
class ManagementTex;
class ManagementSS;
class ManagementMaterial;
class Intersections;
class Rays;
class Geometry;
class Octree;

class Renderer
{
public:
	Renderer();
	~Renderer();

	void render(
			DirectX::XMFLOAT4X4 p_viewMatrix,
			DirectX::XMFLOAT4X4 p_viewMatrixInv,
			DirectX::XMFLOAT4X4 p_projMatrix,
			DirectX::XMFLOAT4X4 p_projMatrixInv,
			DirectX::XMFLOAT3   p_cameraPosition);

	HRESULT init( HWND p_windowHandle, unsigned int p_screenWidth, unsigned int p_screenHeight );
private:
	HRESULT initManagementD3D(  HWND p_windowHandle, unsigned int p_screenWidth, unsigned int p_screenHeight );
	HRESULT initManagementShader( ID3D11Device* p_device );
	HRESULT initManagementCB(ID3D11Device* p_device);
	HRESULT initManagementLight(ID3D11Device* p_device);
	HRESULT initManagementTex(ID3D11Device* p_device, ID3D11DeviceContext* p_context, std::vector<std::wstring> p_texFilenames);
	HRESULT initManagementSS(ID3D11Device* p_device);
	HRESULT initIntersections(ID3D11Device* p_device, unsigned int p_screenWidth, unsigned int p_screenHeight);
	HRESULT initRays(ID3D11Device* p_device, unsigned int p_screenWidth, unsigned int p_screenHeight);
	HRESULT initGeometry(ID3D11Device* p_device, std::vector<Triangle> p_triangles);
	HRESULT initManagementMaterial(ID3D11Device* p_device, std::vector<Material> p_materials);

	HRESULT loadObj(ID3D11Device* p_device, ID3D11DeviceContext* p_context);
	void initOctree(std::vector<Triangle> p_triangles);


	void primaryRayStage();
	void intersectionStage();
	void colorStage();

	unsigned int m_threadCountX;
	unsigned int m_threadCountY;

	ManagementD3D*		m_managementD3D;
	ManagementShader*	m_managementShader;
	ManagementCB*		m_managementCB;
	ManagementLight*	m_managementLight;
	ManagementTex*		m_managementTex;
	ManagementSS*		m_managementSS;
	ManagementMaterial* m_managementMaterial;
	Intersections*		m_intersections;
	Rays*				m_rays;
	Geometry*			m_geometry;
	Octree*				m_octree;
};

#endif //RAYTRACER_RENDERER_H