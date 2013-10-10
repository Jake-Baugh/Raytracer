#ifndef RAYTRACER_MODEL_H
#define RAYTRACER_MODEL_H

#include <vector>
#include <string>

#include <d3d11.h>

#include "Triangle.h"

class Geometry
{
public:
	Geometry();
	~Geometry();

	void csSetSRV(ID3D11DeviceContext* p_context, unsigned int p_startSlot);

	unsigned int getNumTriangles();

	HRESULT init(ID3D11Device* p_device, std::string p_fileName);
private:
	void	initTriangles(std::string p_fileName);
	HRESULT initTriangleBuffer(ID3D11Device* p_device);
	HRESULT initTriangleSRV(ID3D11Device* p_devicve);

	std::vector<Triangle> m_triangles;

	ID3D11Buffer*			  m_triangleBuffer;
	ID3D11ShaderResourceView* m_triangleSRV;
};

#endif //RAYTRACER_MODEL_H