#ifndef RAYTRACER_INPUT_DESC_H
#define RAYTRACER_INPUT_DESC_H

#include <vector>

struct InputDesc
{
	std::vector<bool> m_keys;
	int	m_mouseDeltaX;
	int	m_mouseDeltaY;

	InputDesc()
	{
		m_keys.resize(0, false);
		m_mouseDeltaX = 0;
		m_mouseDeltaY = 0;
	}

	InputDesc( std::vector<bool> p_keys, int p_mouseDeltaX, int p_mouseDeltaY )
	{
		m_keys		  = p_keys;
		m_mouseDeltaX = p_mouseDeltaX;
		m_mouseDeltaY = p_mouseDeltaY;
	}
};

#endif //RAYTRACER_INPUT_DESC_H