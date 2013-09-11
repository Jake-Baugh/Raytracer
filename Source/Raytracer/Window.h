#ifndef RAYTRACER_WINDOW_H
#define RAYTRACER_WINDOW_H

#include <vector>
#include <Windows.h>
#include <windowsx.h>

#include "InputDesc.h"

class Window
{
public:
	Window();
	~Window();

	void checkMessages();
	bool isActive() const;

	unsigned int getScreenWidth() const;
	unsigned int getScreenHeight() const;

	HWND getWindowHandle() const;

	const InputDesc getInput();

	HRESULT init( HINSTANCE p_hInstance, int p_cmdShow );
private:

	HRESULT initWindow( HINSTANCE p_hInstance, int p_cmdShow );
	void	initCursor();

	static void mouseDeltaMove( LPARAM p_lParam );
	static LRESULT CALLBACK WindowProc( HWND p_hWnd, UINT p_message, WPARAM p_wParam, LPARAM p_lParam );

	bool m_active;

	static unsigned int s_screenWidth;
	static unsigned int s_screenHeight;

	static HWND s_windowHandle;

	static std::vector<bool> s_keys;

	static int s_mouseDeltaX;
	static int s_mouseDeltaY;
};

#endif // RAYTRACER_WINDOW_H