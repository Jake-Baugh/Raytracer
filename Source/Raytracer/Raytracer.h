#ifndef RAYTRACER_RAYTRACER_H
#define RAYTRACER_RAYTRACER_H

#include <Windows.h>

class Window;
class Renderer;
class Camera;

class Raytracer
{
public:
	Raytracer();
	~Raytracer();

	void run();

	HRESULT init( HINSTANCE p_hInstance, int p_cmdShow );
private:
	HRESULT initWindow( HINSTANCE p_hInstance, int p_cmdShow );
	HRESULT initRenderer();
	void	initCamera();

	void handleInput(float p_dt);

	Window*		m_window;
	Renderer*	m_renderer;
	Camera*		m_camera;
};

#endif //RAYTRACER_RAYTRACER_H