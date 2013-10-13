#ifndef RAYTRACER_OCTREE_H
#define RAYTRACER_OCTREE_H

#include <vector>
#include <d3d11.h>

#include "Node.h"
#include "Triangle.h"

class Octree
{
public:
	Octree();
	~Octree();

	void csSetNodeSRV(ID3D11DeviceContext* p_context, unsigned int p_startSlot);

	HRESULT init(ID3D11Device* p_device, std::vector<Triangle> p_triangles);
private:
	void buildTree(std::vector<Triangle> p_triangles);
	void flattenTree();

	HRESULT initNodeBuffer(ID3D11Device* p_device);
	HRESULT initNodeSRV(ID3D11Device* p_device);

	LinkedNode* m_root;
	std::vector<Node> m_nodes;

	ID3D11Buffer* m_nodeBuffer;
	ID3D11ShaderResourceView* m_nodeSRV;

	ID3D11Buffer* m_indexBuffer;
	ID3D11UnorderedAccessView* m_indexUAV;
};

#endif //RAYTRACER_OCTREE_H