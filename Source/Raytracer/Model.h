#ifndef RAYTRACER_MODEL_H
#define RAYTRACER_MODEL_H

#include <vector>
#include <string>

#include <d3d11.h>

#include "Vertex.h"

class Model
{
public:
	Model();
	~Model();

	void csSetSRV(ID3D11DeviceContext* p_context, unsigned int p_startSlot);

	unsigned int getNumVertices();

	HRESULT init(ID3D11Device* p_device, std::string p_fileName);
private:
	void	initVertices(std::string p_fileName);
	HRESULT initVertexBuffer(ID3D11Device* p_device);
	HRESULT initVertexSRV(ID3D11Device* p_devicve);

	std::vector<Vertex> m_vertices;

	ID3D11Buffer* m_vertexBuffer;
	ID3D11ShaderResourceView* m_vertexSRV;
};

#endif //RAYTRACER_MODEL_H