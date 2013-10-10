#ifndef RAYTRACER_MANAGEMENT_TEX_H
#define RAYTRACER_MANAGEMENT_TEX_H

#include <string>
#include <vector>
#include <d3d11.h>
#include "TexIds.h"

class ManagementTex
{
public:
	ManagementTex();
	~ManagementTex();

	void csSetTexture(ID3D11DeviceContext* p_context, unsigned int p_startSlot);

	HRESULT init(ID3D11Device* p_device, ID3D11DeviceContext* p_context, std::vector<std::wstring> p_texFilenames );
private:
	HRESULT loadTexture(ID3D11Device* p_device,
		ID3D11DeviceContext* p_context,
		std::wstring p_fileName,
		unsigned int p_arraySlice);

	HRESULT initTextureArray(ID3D11Device* p_device, unsigned int p_texCnt);

	ID3D11Texture2D* m_textureArray;
	ID3D11ShaderResourceView* m_textureArraySRV;
};

#endif //RAYTRACER_MANAGEMENT_TEX_H