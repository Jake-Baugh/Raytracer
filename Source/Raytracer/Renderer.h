#ifndef RAYTRACER_RENDERER_H
#define RAYTRACER_RENDERER_H

#include <d3d11.h>
#include <DirectXMath.h>

class ManagementD3D;
class ManagementShader;
class ManagementCB;
class ManagementLight;
class ManagementTex;
class ManagementSS;
class Intersections;
class Rays;
class Model;

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
	HRESULT initManagementTex(ID3D11Device* p_device);
	HRESULT initManagementSS(ID3D11Device* p_device);
	HRESULT initIntersections(ID3D11Device* p_device, unsigned int p_screenWidth, unsigned int p_screenHeight);
	HRESULT initRays(ID3D11Device* p_device, unsigned int p_screenWidth, unsigned int p_screenHeight);
	HRESULT initCube(ID3D11Device* p_device);


	void primaryRayStage();
	void intersectionStage();
	void colorStage();

	ManagementD3D*		m_managementD3D;
	ManagementShader*	m_managementShader;
	ManagementCB*		m_managementCB;
	ManagementLight*	m_managementLight;
	ManagementTex*		m_managementTex;
	ManagementSS*		m_managementSS;
	Intersections*		m_intersections;
	Rays*				m_rays;

	Model*				m_cube;
};

#endif //RAYTRACER_RENDERER_H