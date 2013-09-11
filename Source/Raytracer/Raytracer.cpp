#include "Raytracer.h"

#include "Camera.h"
#include "Window.h"
#include "Renderer.h"
#include "Utility.h"
#include "KeyCodes.h"

Raytracer::Raytracer()
{
	m_window	= nullptr;
	m_renderer	= nullptr;
	m_camera	= nullptr;
}
Raytracer::~Raytracer()
{
	SAFE_DELETE( m_window );
	SAFE_DELETE( m_renderer );
	SAFE_DELETE( m_camera );
}

void Raytracer::run()
{
	LARGE_INTEGER freq, old, current;
	QueryPerformanceFrequency (&freq);
	QueryPerformanceCounter (&old);

	float dt, fps;

	while( m_window->isActive() )
	{
		QueryPerformanceCounter(&current);
		dt = float (current.QuadPart - old.QuadPart) / float(freq.QuadPart);
		fps = 1/dt;

		m_window->checkMessages();

		handleInput(dt);

		m_renderer->render(
			m_camera->getViewMatrix(),
			m_camera->getViewMatrixInv(),
			m_camera->getProjMatrix(),
			m_camera->getProjMatrixInv(),
			m_camera->getPosition() );

		old.QuadPart = current.QuadPart;
	}
}

HRESULT Raytracer::init( HINSTANCE p_hInstance, int p_cmdShow )
{
	HRESULT hr = S_OK;
	hr = initWindow( p_hInstance, p_cmdShow );
	if(SUCCEEDED(hr))
		hr = initRenderer();

	if(SUCCEEDED(hr))
		initCamera();

	return hr;
}
HRESULT Raytracer::initWindow( HINSTANCE p_hInstance, int p_cmdShow )
{
	HRESULT hr = S_OK;
	m_window = new Window();
	m_window->init( p_hInstance, p_cmdShow );
	return hr;
}
HRESULT Raytracer::initRenderer()
{
	HRESULT hr = S_OK;
	m_renderer = new Renderer();
	m_renderer->init(
		m_window->getWindowHandle(),
		m_window->getScreenWidth(),
		m_window->getScreenHeight() );

	return hr;
}
void	Raytracer::initCamera()
{
	m_camera = new Camera();
	float aspectRatio = static_cast<float>(m_window->getScreenWidth())/
						static_cast<float>(m_window->getScreenHeight());

	m_camera->setLens(DirectX::XM_PIDIV4, aspectRatio, 1.0f, 1000.0f);
}

void Raytracer::handleInput(float p_dt)
{
	InputDesc input = m_window->getInput();

	float distance = 10.0f * p_dt; 

	if(input.m_keys[KeyCodes::VK_W])
		m_camera->walk(distance);
	if(input.m_keys[KeyCodes::VK_A])
		m_camera->strafe(-distance);
	if(input.m_keys[KeyCodes::VK_S])
		m_camera->walk(-distance);
	if(input.m_keys[KeyCodes::VK_D])
		m_camera->strafe(distance);
	if(input.m_keys[VK_SPACE])
		m_camera->verticalWalk(distance);
	if(input.m_keys[VK_CONTROL])
		m_camera->verticalWalk(-distance);

	m_camera->yaw(input.m_mouseDeltaX * p_dt);
	m_camera->pitch(input.m_mouseDeltaY * p_dt);

	m_camera->rebuildView();
}