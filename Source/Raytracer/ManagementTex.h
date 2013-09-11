#ifndef RAYTRACER_MANAGEMENT_TEX_H
#define RAYTRACER_MANAGEMENT_TEX_H

#include <string>
#include <d3d11.h>
#include "TexIds.h"

class ManagementTex
{
public:
	ManagementTex();
	~ManagementTex();

	void csSetTexture(ID3D11DeviceContext* p_context, TexIds::Id p_texId, unsigned int p_startSlot);

	HRESULT init(ID3D11Device* p_device);
private:
	HRESULT loadTexture(ID3D11Device* p_device, std::wstring p_fileName, TexIds::Id p_texId);

	ID3D11ShaderResourceView* m_srvTextures[TexIds::TexIds_COUNT];
};

#endif //RAYTRACER_MANAGEMENT_TEX_H