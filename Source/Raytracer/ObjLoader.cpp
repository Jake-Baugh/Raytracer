#include "ObjLoader.h"
#include <Windows.h>

ObjLoader::ObjLoader()
{
	m_ambient	= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_diffuse	= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_specular	= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	m_specularCoefficient	= 0.0f;
	m_transparency			= 0.0f;

	m_ambientTexName			= L"Unknown";
	m_diffuseTexName			= L"Unknown";
	m_specularTexName			= L"Unknown";
	m_specularHighlightTexName	= L"Unknown";
	m_alphaTexName				= L"Unknown";
	m_bumpMapName				= L"Unknown";
	m_displacementMapName		= L"Unknown";
	m_stencilDecalMapName		= L"Unknown";
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
				parseMtlFile(infile);
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
	m_positions.push_back(readFloat3(p_infile));
}
void ObjLoader::parseNormal(std::fstream& p_infile)
{
	m_normals.push_back(readFloat3(p_infile));
}
void ObjLoader::parseTexCoord(std::fstream& p_infile)
{
	m_texCoords.push_back(readFloat2(p_infile));
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

void ObjLoader::parseMtlFile(std::fstream& p_infile)
{
	std::string mtlFilename;
	p_infile >> mtlFilename;

	std::fstream mtlFile(mtlFilename);

	std::string type;
	std::string str;

	if(mtlFile)
	{
		while(!mtlFile.eof())
		{
			type = "unknown";

			mtlFile >> type;
			if(type == "Ka")
				parseAmbient(mtlFile);
			else if(type == "Kd")
				parseDiffuse(mtlFile);
			else if(type == "Ks")
				parseSpecular(mtlFile);
			else if(type == "Ns")
				parseSpecularCoefficient(mtlFile);
			else if(type == "d" || type == "Tr")
				parseTransparency(mtlFile);
			else if(type == "map_Ka")
				parseAmbientTex(mtlFile);
			else if(type == "map_Kd")
				parseDiffuseTex(mtlFile);
			else if(type == "map_Ks")
				parseSpecularTex(mtlFile);
			else if(type == "map_Ns")
				parseSpecularHighlightTex(mtlFile);
			else if(type == "map_d")
				parseAlphaTexture(mtlFile);
			else if(type == "map_bump" || type == "bump")
				parseBumpMap(mtlFile);
			else if(type == "disp")
				parseDisplacementMap(mtlFile);
			else if(type == "decal")
				parseStencilDecalMap(mtlFile);
		}
	}
}
void ObjLoader::parseAmbient(std::fstream& p_mtlFile)
{
	m_ambient = readFloat3(p_mtlFile);
}
void ObjLoader::parseDiffuse(std::fstream& p_mtlFile)
{
	m_diffuse = readFloat3(p_mtlFile);
}
void ObjLoader::parseSpecular(std::fstream& p_mtlFile)
{
	m_specular = readFloat3(p_mtlFile);
}
void ObjLoader::parseSpecularCoefficient(std::fstream& p_mtlFile)
{
	m_specularCoefficient = readFloat(p_mtlFile);
}
void ObjLoader::parseTransparency(std::fstream& p_mtlFile)
{
	m_transparency = readFloat(p_mtlFile);
}

void ObjLoader::parseAmbientTex(std::fstream& p_mtlFile)
{
	m_ambientTexName = readWString(p_mtlFile);
}
void ObjLoader::parseDiffuseTex(std::fstream& p_mtlFile)
{
	m_diffuseTexName = readWString(p_mtlFile);
}
void ObjLoader::parseSpecularTex(std::fstream& p_mtlFile)
{
	m_specularTexName = readWString(p_mtlFile);
}
void ObjLoader::parseSpecularHighlightTex(std::fstream& p_mtlFile)
{
	m_specularHighlightTexName = readWString(p_mtlFile);
}
void ObjLoader::parseAlphaTexture(std::fstream& p_mtlFile)
{
	m_alphaTexName = readWString(p_mtlFile);
}
void ObjLoader::parseBumpMap(std::fstream& p_mtlFile)
{
	m_bumpMapName = readWString(p_mtlFile);
}
void ObjLoader::parseDisplacementMap(std::fstream& p_mtlFile)
{
	m_displacementMapName = readWString(p_mtlFile); 
}
void ObjLoader::parseStencilDecalMap(std::fstream& p_mtlFile)
{
	m_stencilDecalMapName = readWString(p_mtlFile);
}

float ObjLoader::readFloat(std::fstream& p_file)
{
	float f;
	p_file >> f;
	return f;
}
DirectX::XMFLOAT2 ObjLoader::readFloat2(std::fstream& p_file)
{
	float x, y;
	p_file >> x >> y;
	return DirectX::XMFLOAT2(x, y);
}
DirectX::XMFLOAT3 ObjLoader::readFloat3(std::fstream& p_file)
{
	float x, y, z;
	p_file >> x >> y >> z;
	return DirectX::XMFLOAT3(x, y, z);
}
std::wstring ObjLoader::readWString(std::fstream& p_file)
{
	std::string str;
	p_file >> str;
	std::wstring wStr(str.length(), L'');
	copy(str.begin(), str.end(), wStr.begin());
	return wStr;
}