#include "Octree.h"
#include "Utility.h"

Octree::Octree()
{
	m_nodeBuffer = nullptr;
	m_nodeSRV	 = nullptr;

	m_indexBuffer = nullptr;
	m_indexUAV	  = nullptr;
}
Octree::~Octree()
{
	SAFE_DELETE(m_root);
	SAFE_RELEASE(m_nodeBuffer);
	SAFE_RELEASE(m_nodeSRV);
	SAFE_RELEASE(m_indexBuffer);
	SAFE_RELEASE(m_indexUAV);
}

void Octree::csSetNodeSRV(ID3D11DeviceContext* p_context, unsigned int p_startSlot)
{
	p_context->CSSetShaderResources(p_startSlot, 1, &m_nodeSRV);
}

HRESULT Octree::init(ID3D11Device* p_device, std::vector<Triangle> p_triangles)
{
	HRESULT hr = S_OK;

	buildTree(p_triangles);
	flattenTree();

	hr = initNodeBuffer(p_device);
	if(SUCCEEDED(hr))
		hr = initNodeSRV(p_device);

	return hr;
}

void Octree::buildTree(std::vector<Triangle> p_triangles)
{
	float minX = FLT_MAX;
	float minY = FLT_MAX;
	float minZ = FLT_MAX;

	float maxX = -FLT_MAX;
	float maxY = -FLT_MAX;
	float maxZ = -FLT_MAX;

	for(unsigned int triIdx=0; triIdx<p_triangles.size(); triIdx++)
	{
		for(unsigned int i=0; i<3; i++)
		{
			DirectX::XMFLOAT3 pos = p_triangles[triIdx].vertices[i].m_position;

			if(pos.x > maxX)
				maxX = pos.x;
			if(pos.y > maxY)
				maxY = pos.y;
			if(pos.z > maxZ)
				maxZ = pos.z;
			
			if(pos.x < minX)
				minX = pos.x;
			if(pos.y < minY)
				minY = pos.y;
			if(pos.z < minZ)
				minZ = pos.z;
		}
	}

	DirectX::XMFLOAT3 min = DirectX::XMFLOAT3(minX-1, minY-1, minZ-1);
	DirectX::XMFLOAT3 max = DirectX::XMFLOAT3(maxX+1, maxY+1, maxZ+1);

	std::vector<unsigned int> indices;
	for(unsigned int i=0; i<p_triangles.size(); i++)
		indices.push_back(i);

	m_root = new LinkedNode(min, max);
	m_root->subdivide(min, max, indices, p_triangles);
}
void Octree::flattenTree()
{
	unsigned int currentNodeIndex = 0;

	LinkedNode* currentNode = m_root;
	LinkedNode* child		= nullptr;

	std::vector<LinkedNode*> linkedNodes;
	linkedNodes.push_back(currentNode);

	bool done = false;
	while(!done)
	{
		for(unsigned int i=0; i<NUM_CHILDREN; i++)
		{
			child = currentNode->getChild(i);
			if(child)
			{
				linkedNodes.push_back(child);
				currentNode->addChildIndex(linkedNodes.size()-1);
			}
		}
		currentNodeIndex++;
		if(currentNodeIndex < linkedNodes.size())
			currentNode = linkedNodes[currentNodeIndex];
		else
			done = true;
	}

	for(unsigned int i=0; i<linkedNodes.size(); i++)
	{
		m_nodes.push_back(
		Node(
		linkedNodes[i]->getMin(),
		linkedNodes[i]->getMax(),
		linkedNodes[i]->getTriIndices(), 
		linkedNodes[i]->getChildIndices()));
	}
}

HRESULT Octree::initNodeBuffer(ID3D11Device* p_device)
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.BindFlags			= D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.ByteWidth			= sizeof(Node) * m_nodes.size();
	bufferDesc.MiscFlags			= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride	= sizeof(Node);

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &m_nodes[0];

	hr = p_device->CreateBuffer(&bufferDesc, &initData, &m_nodeBuffer);
	if(FAILED(hr))
	{
		MessageBox(
			0,
			L"Model::initVertexBuffer | CreateBuffer() Failed",
			L"Model.h",
			MB_OK | MB_ICONEXCLAMATION);
	}

	return hr;
}
HRESULT Octree::initNodeSRV(ID3D11Device* p_device)
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	m_nodeBuffer->GetDesc(&bufferDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));

	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.BufferEx.NumElements = bufferDesc.ByteWidth / bufferDesc.StructureByteStride;

	hr = p_device->CreateShaderResourceView(m_nodeBuffer, &srvDesc, &m_nodeSRV);
	if(FAILED(hr))
	{
		MessageBox(
			0,
			L"Model::initVertexSRV | CreateShaderResourceView() Failed",
			L"Model.h",
			MB_OK | MB_ICONEXCLAMATION);
	}

	return hr;
}
