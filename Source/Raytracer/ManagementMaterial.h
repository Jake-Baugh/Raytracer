#ifndef RAYTRACER_MANAGEMENT_MATERIAL_H
#define RAYTRACER_MANAGEMENT_MATERIAL_H

#include <d3d11.h>
#include <vector>
#include "Material.h"

class ManagementMaterial
{
public:
	ManagementMaterial();
	~ManagementMaterial();
	
	void csSetSRV(ID3D11DeviceContext* p_context, unsigned int p_startSlot);

	HRESULT init(ID3D11Device* p_device, std::vector<Material> p_materials);
private:

	HRESULT initMaterialBuffer(ID3D11Device* p_device);
	HRESULT initMaterialSRV(ID3D11Device* p_device);

	std::vector<Material> m_materials;

	ID3D11Buffer* m_materialBuffer;
	ID3D11ShaderResourceView* m_materialSRV;
};

#endif //RAYTRACER_MANAGEMENT_MATERIAL_H