#include <Windows.h>

#if defined( DEBUG ) || defined( _DEBUG )
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include "Raytracer.h"

int WINAPI WinMain( HINSTANCE p_hInstance, HINSTANCE p_hPrevInstance, LPSTR p_lpCmdLine, int p_cmdShow )
{
#if defined( DEBUG ) || defined( _DEBUG )
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );
#endif

	AllocConsole();

	HRESULT hr = S_OK;

	Raytracer raytracer;
	hr = raytracer.init( p_hInstance, p_cmdShow );

	if(SUCCEEDED(hr))
		raytracer.run();

	return 0;
}