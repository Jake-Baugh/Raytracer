#ifndef RAYTRACER_TRIANGLE_H
#define RAYTRACER_TRIANGLE_H

#include "Vertex.h"

struct Triangle 
{
	Vertex vertices[3];
	int materialIndex;

	Triangle()
	{
		materialIndex = -1;
	}
	Triangle(int p_materialIndex)
	{
		materialIndex = p_materialIndex;
	}
};

#endif //RAYTRACER_TRIANGLE_H