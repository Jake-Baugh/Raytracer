#ifndef RAYTRACER_MANAGEMENT_SHADER_H
#define RAYTRACER_MANAGEMENT_SHADER_H

#include <string>
#include <d3d11.h>
#include <d3dcompiler.h>

class ManagementShader
{
public:
	ManagementShader();
	~ManagementShader();

	enum CSIds
	{
		CSIds_PRIMARY_RAY_STAGE,
		CSIds_INTERSECTION_STAGE,
		CSIds_COLOR_STAGE,
		CSIds_COUNT
	};

	void csSetShader( ID3D11DeviceContext* p_context, CSIds p_shaderId );

	HRESULT init( ID3D11Device* p_device );
private:
	HRESULT initCS(
		ID3D11Device* p_device,
		std::wstring p_fileName,
		ID3D11ComputeShader** p_shader,
		ID3DBlob** p_blob );

	ID3D11ComputeShader* m_computeShaders[CSIds_COUNT];
	ID3DBlob*			 m_csBlobs[CSIds_COUNT];
};

#endif //RAYTRACER_MANAGEMENT_SHADER_H