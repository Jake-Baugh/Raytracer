#ifndef RAYTRACER_MANAGEMENT_SS_H
#define RAYTRACER_MANAGEMENT_SS_H

#include <vector>
#include <d3d11.h>

class ManagementSS
{
public:
	ManagementSS();
	~ManagementSS();

	enum SSTypes
	{
		SSTypes_DEFAULT,
		SSTypes_WRAP,
		SSTypes_CNT
	};

	void csSetSS(ID3D11DeviceContext* devcon, SSTypes ssType, unsigned int sahderRegister);

	HRESULT init(ID3D11Device* p_device);
private:
	HRESULT initSSDefault(ID3D11Device* p_device);
	HRESULT initSSWrap( ID3D11Device* p_device );

	std::vector<ID3D11SamplerState*> m_samplerStates;
};

#endif //RAYTRACER_MANAGEMENT_SS_H