#include "ManagementD3D.h"

#include "Utility.h"

ManagementD3D::ManagementD3D()
{
	m_swapChain			 = nullptr;
	m_device			 = nullptr;
	m_context			 = nullptr;
	m_uavBackBuffer		 = nullptr;
	m_accumulationBuffer = nullptr;
	m_accumulationUav	 = nullptr;
}
ManagementD3D::~ManagementD3D()
{
	SAFE_RELEASE( m_swapChain );
	SAFE_RELEASE( m_device );
	SAFE_RELEASE( m_context );
	SAFE_RELEASE( m_uavBackBuffer );
	SAFE_RELEASE( m_accumulationBuffer );
	SAFE_RELEASE( m_accumulationUav );
}

void ManagementD3D::present()
{
	m_swapChain->Present(0, 0);
}

ID3D11Device*		 ManagementD3D::getDevice() const
{
	return m_device;
}
ID3D11DeviceContext* ManagementD3D::getDeviceContext() const
{
	return m_context;
}

void ManagementD3D::setBackBuffer()
{
	m_context->CSSetUnorderedAccessViews(0, 1, &m_uavBackBuffer, nullptr);
}
void ManagementD3D::setAccumulationUAV(unsigned int p_startSlot)
{
	m_context->CSSetUnorderedAccessViews(p_startSlot, 1, &m_accumulationUav, nullptr); 
}

HRESULT ManagementD3D::init( HWND p_windowHandle, unsigned int p_screenWidth, unsigned int p_screenHeight )
{
	HRESULT hr = S_OK;

	hr = initDeviceAndSwapChain(p_windowHandle, p_screenWidth, p_screenHeight);
	if(SUCCEEDED(hr))
		hr = initBackBuffer();
	if(SUCCEEDED(hr))
		hr = initAccumulationBuffer(p_screenWidth, p_screenHeight);
	if(SUCCEEDED(hr))
		hr = initAccumulationUav();
	return hr;
}
HRESULT ManagementD3D::initDeviceAndSwapChain( HWND p_windowHandle, unsigned int p_screenWidth, unsigned int p_screenHeight )
{
	HRESULT hr = S_OK;

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(scd));

	scd.BufferCount							= 1;
	scd.BufferDesc.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Width					= p_screenWidth;
	scd.BufferDesc.Height					= p_screenHeight;
	scd.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS;
	scd.OutputWindow						= p_windowHandle;
	scd.BufferDesc.RefreshRate.Numerator	= 60;
	scd.BufferDesc.RefreshRate.Denominator	= 1;
	scd.SampleDesc.Count					= 1;
	scd.SampleDesc.Quality					= 0;
	scd.Windowed							= true;
	scd.Flags								= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	UINT numFeatureLevels = 3;
	D3D_FEATURE_LEVEL initiatedFeatureLevel;
	D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0};

	UINT numDriverTypes = 2;
	D3D_DRIVER_TYPE driverTypes[] = {D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_REFERENCE};

	UINT createDeviceFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	unsigned int index = 0;
	bool deviceCreated = false;
	while( index < numDriverTypes && !deviceCreated )
	{
		hr = D3D11CreateDeviceAndSwapChain(
				0,
				driverTypes[index],
				0,
				createDeviceFlags,
				featureLevels,
				numFeatureLevels,
				D3D11_SDK_VERSION,
				&scd,
				&m_swapChain,
				&m_device,
				&initiatedFeatureLevel,
				&m_context);
	
		if(SUCCEEDED(hr))
			deviceCreated = true;
		else
			index++;
	}

	if(FAILED(hr))
	{
		MessageBox(nullptr,
		L"D3D11CreateDeviceAndSwapChain() Failed!",
		L"ManagementD3D",
		MB_OK | MB_ICONEXCLAMATION);
	}
	return hr;
}
HRESULT ManagementD3D::initBackBuffer()
{
	HRESULT hr = S_OK;

	ID3D11Texture2D* texBackBuffer;
	m_swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (LPVOID*)&texBackBuffer );
	hr = m_device->CreateUnorderedAccessView( texBackBuffer, nullptr, &m_uavBackBuffer );
	SAFE_RELEASE(texBackBuffer);
	return hr;
}
HRESULT ManagementD3D::initAccumulationBuffer(unsigned int p_width, unsigned int p_height)
{
	HRESULT hr = S_OK;
	
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	int elementSize  = 32; //Four float values
	int elementCount = p_width * p_height;

	bufferDesc.BindFlags			= D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.ByteWidth			= elementSize * elementCount;
	bufferDesc.MiscFlags			= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride  = elementSize;

	hr = m_device->CreateBuffer(&bufferDesc, nullptr, &m_accumulationBuffer);
	if(FAILED(hr))
	{
		MessageBox(
		0,
		L"ManagementD3D::initAccumulationBuffer() | Failed",
		L"ManagementD3D",
		MB_OK | MB_ICONEXCLAMATION);
	}
	return hr;
}
HRESULT ManagementD3D::initAccumulationUav()
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	m_accumulationBuffer->GetDesc(&bufferDesc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));

	uavDesc.ViewDimension		= D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Format				= DXGI_FORMAT_UNKNOWN;
	uavDesc.Buffer.NumElements	= bufferDesc.ByteWidth / bufferDesc.StructureByteStride;

	hr = m_device->CreateUnorderedAccessView(m_accumulationBuffer, &uavDesc, &m_accumulationUav);
	if(FAILED(hr))
	{
		MessageBox(
		0,
		L"ManagementD3D::initAccumulationUav() | Failed",
		L"ManagementD3D",
		MB_OK | MB_ICONEXCLAMATION);
	}

	return hr;
}