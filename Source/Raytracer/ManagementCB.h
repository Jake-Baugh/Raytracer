#ifndef RAYTRACER_MANAGEMENT_CB_H
#define RAYTRACER_MANAGEMENT_CB_H

#include <d3d11.h>
#include <DirectXMath.h>

#include "CBDesc.h"

class ManagementCB
{
public:
	ManagementCB();
	~ManagementCB();

	void csSetCB(ID3D11DeviceContext* p_context, CBIds::Id p_cbId);

	void updateCBFrame(
		ID3D11DeviceContext* p_context,
		DirectX::XMFLOAT4X4  p_viewMatrix,
		DirectX::XMFLOAT4X4  p_viewMatrixInv,
		DirectX::XMFLOAT4X4  p_projMatrix,
		DirectX::XMFLOAT4X4  p_projMatrixInv,
		DirectX::XMFLOAT3	 p_cameraPosition,
		unsigned int		 p_numLights);

	void updateCBWindowResize(
			ID3D11DeviceContext* p_context,
			unsigned int p_screenWidth,
			unsigned int p_screenHeight);

	void updateCBObject(ID3D11DeviceContext* p_context, unsigned int p_numVertices);

	HRESULT init(ID3D11Device* p_device);
private:
	HRESULT initCB(ID3D11Device* p_device, ID3D11Buffer** p_constantBuffer, unsigned int p_size);

	ID3D11Buffer* m_constantBuffers[CBIds::CBIds_COUNT];
};

#endif //RAYTRACER_MANAGEMENT_CB_H