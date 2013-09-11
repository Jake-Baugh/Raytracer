#include "ObjLoader.h"
#include <Windows.h>

ObjLoader::ObjLoader()
{
}
ObjLoader::~ObjLoader()
{
}

std::vector<Vertex> ObjLoader::getLoadedVertices()
{
	return m_vertices;
}

void ObjLoader::loadObj(std::string p_fileName)
{
	m_vertices.clear();
	m_positions.clear();
	m_normals.clear();
	m_texCoords.clear();

	std::fstream infile(p_fileName.c_str());

	if(infile)
	{
		std::string id;
		while(!infile.eof())
		{
			id = "unknown";
			infile >> id;	

			if(id == "v")
				parsePosition(infile);
			else if(id == "vn")
				parseNormal(infile);
			else if(id == "vt")
				parseTexCoord(infile);
			else if(id == "f")
				parseFace(infile);
			else if(id == "mtllib")
				parseMaterial(infile);
		}
	}
	else
	{
		MessageBox(
			0,
			L"ObjLoader::loadObj | Could not open file",
			L"ObjLoader.h",
			MB_OK | MB_ICONEXCLAMATION);
	}
}

void ObjLoader::parsePosition(std::fstream& p_infile)
{
	float x, y, z;
	p_infile >> x >> y >> z;
	m_positions.push_back(DirectX::XMFLOAT3(x, y, z));
}
void ObjLoader::parseNormal(std::fstream& p_infile)
{
	float x, y, z;
	p_infile >> x >> y >> z;
	m_normals.push_back(DirectX::XMFLOAT3(x, y, z));
}
void ObjLoader::parseTexCoord(std::fstream& p_infile)
{
	float u, v;
	p_infile >> u >> v;
	m_texCoords.push_back(DirectX::XMFLOAT2(u, v));
}
void ObjLoader::parseFace(std::fstream& p_infile)
{
	unsigned int iPosition, iNormal, iTexCoord;
	Vertex vertex;

	for(unsigned int i=0; i<3; i++)
	{
		p_infile >> iPosition;
		vertex.m_position = m_positions[iPosition-1];
		if(p_infile.peek() == '/')
		{
			p_infile.ignore();
			if(p_infile.peek() != '/')
			{
				p_infile >> iTexCoord;
				vertex.m_texCoord = m_texCoords[iTexCoord-1];
			}
			if(p_infile.peek() == '/')
			{
				p_infile.ignore();
				p_infile >> iNormal;
				vertex.m_normal = m_normals[iNormal-1];
			}
		}
		m_vertices.push_back(vertex);
	}
}
void ObjLoader::parseMaterial(std::fstream& p_infile)
{
	std::string mtlFilename;
	p_infile >> mtlFilename;

	std::fstream mtlFile(mtlFilename);

	std::string temp;
	std::string str;

	if(mtlFile)
	{
		while(!mtlFile.eof())
		{
			temp = "unknown";

			mtlFile >> temp;
			if(temp == "map_Kd")
			{
				mtlFile >> str;
				std::wstring str2(str.length(), L'');
				copy(str.begin(), str.end(), str2.begin());
				std::wstring textureFilename = str2;
			}
		}
	}
}