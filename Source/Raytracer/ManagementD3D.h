#ifndef RAYTRACER_MANAGEMENTD3D_H
#define RAYTRACER_MANAGEMENTD3D_H

#include <d3d11.h>

class ManagementD3D
{
public:
	ManagementD3D();
	~ManagementD3D();

	void present();

	ID3D11Device*		 getDevice() const;
	ID3D11DeviceContext* getDeviceContext() const;

	void setBackBuffer();
	void setAccumulationUAV(unsigned int p_startSlot);

	HRESULT init( HWND p_windowHandle, unsigned int p_screenWidth, unsigned int p_screenHeight );
private:
	HRESULT initDeviceAndSwapChain( HWND p_windowHandle, unsigned int p_screenWidth, unsigned int p_screenHeight );
	HRESULT initBackBuffer();
	HRESULT initAccumulationBuffer(unsigned int p_width, unsigned int p_height);
	HRESULT initAccumulationUav();

	IDXGISwapChain*				m_swapChain;
	ID3D11Device*				m_device;
	ID3D11DeviceContext*		m_context;
	ID3D11UnorderedAccessView*  m_uavBackBuffer;

	ID3D11Buffer*				m_accumulationBuffer;
	ID3D11UnorderedAccessView*  m_accumulationUav;
	
};

#endif //RAYTRACER_MANAGEMENTD3D_H