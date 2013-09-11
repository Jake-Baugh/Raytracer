#include "Window.h"

#include "KeyCodes.h"

std::vector<bool> Window::s_keys;

unsigned int Window::s_screenWidth;
unsigned int Window::s_screenHeight;

int	 Window::s_mouseDeltaX;
int	 Window::s_mouseDeltaY;
HWND Window::s_windowHandle;

Window::Window()
{
	m_active = true;

	s_keys.resize( KeyCodes::NUM_KEYS, false );
	s_mouseDeltaX = 0;
	s_mouseDeltaY = 0;

	s_screenWidth  = 800;
	s_screenHeight = 800;
}
Window::~Window()
{
	ShowCursor( true );
	ReleaseCapture();
}

void Window::checkMessages()
{
	MSG msg = {0};
	if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );

		if(msg.message == WM_QUIT)
			m_active = false;
	}
}

bool Window::isActive() const
{
	return m_active;
}

unsigned int Window::getScreenWidth() const
{
	return s_screenWidth;
}
unsigned int Window::getScreenHeight() const
{
	return s_screenHeight;
}

HWND Window::getWindowHandle() const
{
	return s_windowHandle;
}

const InputDesc Window::getInput()
{
	return InputDesc(s_keys, s_mouseDeltaX, s_mouseDeltaY);
}

HRESULT Window::init( HINSTANCE p_hInstance, int p_cmdShow )
{
	HRESULT hr = S_OK;

	hr = initWindow( p_hInstance, p_cmdShow );
	if(SUCCEEDED(hr))
		initCursor();

	return hr;
}
HRESULT Window::initWindow( HINSTANCE p_hInstance, int p_cmdShow )
{
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));

	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.style		 = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc	 = WindowProc;
	wc.hInstance	 = p_hInstance;
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"WindowClass";

	RegisterClassEx(&wc);

	unsigned int windowPosX = 10;
	unsigned int windowPosY = 10;
	DWORD		 dwExStyle	= 0;
	HWND		 parent		= 0;
	HMENU		 menu		= 0;
	LPVOID		 lParam		= 0;

	s_windowHandle = CreateWindowEx(
		dwExStyle, 
		L"WindowClass",
		L"Raytracer",
		WS_OVERLAPPEDWINDOW,
		windowPosX,
		windowPosY,
		s_screenWidth,
		s_screenHeight,
		parent,
		menu,
		p_hInstance,
		lParam );

	HRESULT hr = S_OK;
	if(!s_windowHandle)
	{
		hr = E_FAIL;
		MessageBox(NULL,
			L"Window could not be created!",
			L"Window Error!",
			MB_OK | MB_ICONEXCLAMATION);
	}
	else
		ShowWindow(s_windowHandle, p_cmdShow);

	return hr;
}
void	Window::initCursor()
{
	SetCursorPos( s_screenWidth/2, s_screenHeight/2 );
	SetCapture( s_windowHandle );
	ShowCursor( false );
}

void Window::mouseDeltaMove( LPARAM p_lParam )
{
	//Find the upper left corner of the window's client area in screen coordinates.
	POINT point;
	point.x = 0;
	point.y = 0;
	MapWindowPoints(s_windowHandle, 0, &point, 1);

	//Get current mouse position.
	int mouseX = GET_X_LPARAM(p_lParam)+point.x;
	int mouseY = GET_Y_LPARAM(p_lParam)+point.y;

	//Calculate relative mouse movement.
	s_mouseDeltaX = mouseX - s_screenWidth/2;
	s_mouseDeltaY = mouseY - s_screenHeight/2;

	//Return cursor to center of screen.
	SetCursorPos( s_screenWidth/2, s_screenHeight/2 );
}

LRESULT CALLBACK Window::WindowProc( HWND p_hWnd, UINT p_message, WPARAM p_wParam, LPARAM p_lParam )
{
	switch(p_message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		break;
	case WM_KEYDOWN:
		if(p_wParam == VK_ESCAPE)
			DestroyWindow(p_hWnd);
		s_keys[p_wParam] = true;
		return 0;
		break;
	case WM_KEYUP:
		s_keys[p_wParam] = false;
		return 0;
		break;
	case WM_MOUSEMOVE:
		mouseDeltaMove(p_lParam);
		return 0;
		break;
	}

	return DefWindowProc( p_hWnd, p_message, p_wParam, p_lParam);
}