#ifndef RAYTRACER_OBJ_LOADER_H
#define RAYTRACER_OBJ_LOADER_H

#include <fstream>
#include <string>
#include <vector>

#include <DirectXMath.h>

#include "Vertex.h"

class ObjLoader
{
public:
	ObjLoader();
	~ObjLoader();

	std::vector<Vertex> getLoadedVertices();

	void loadObj(std::string p_fileName);
private:
	void parsePosition(std::fstream& p_infile);
	void parseNormal(std::fstream& p_infile);
	void parseTexCoord(std::fstream& p_infile);
	void parseFace(std::fstream& p_infile);
	void parseMaterial(std::fstream& p_infile);

	std::vector<Vertex>				m_vertices;
	std::vector<DirectX::XMFLOAT3>	m_positions;
	std::vector<DirectX::XMFLOAT3>	m_normals;
	std::vector<DirectX::XMFLOAT2>	m_texCoords;

};

#endif //RAYTRACER_OBJ_LOADER_H