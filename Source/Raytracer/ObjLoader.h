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
	
	void parseMtlFile(std::fstream& p_infile);

	void parseAmbient(std::fstream& p_mtlfile);
	void parseDiffuse(std::fstream& p_mtlFile);
	void parseSpecular(std::fstream& p_mtlFile);
	void parseSpecularCoefficient(std::fstream& p_mtlFile);
	void parseTransparency(std::fstream& p_mtlFile);
	void parseIndexOfRefraction(std::fstream& p_mtlFile);

	void parseAmbientTex(std::fstream& p_mtlFile);
	void parseDiffuseTex(std::fstream& p_mtlFile);
	void parseSpecularTex(std::fstream& p_mtlFile);
	void parseSpecularHighlightTex(std::fstream& p_mtlFile);
	void parseAlphaTexture(std::fstream& p_mtlFile);
	void parseBumpMap(std::fstream& p_mtlFile);
	void parseDisplacementMap(std::fstream& p_mtlFile);
	void parseStencilDecalMap(std::fstream& p_mtlFile);

	float				readFloat(std::fstream& p_file);
	DirectX::XMFLOAT2	readFloat2(std::fstream& p_file);
	DirectX::XMFLOAT3	readFloat3(std::fstream& p_file);
	std::wstring		readWString(std::fstream& p_file);

	std::vector<Vertex>				m_vertices;
	std::vector<DirectX::XMFLOAT3>	m_positions;
	std::vector<DirectX::XMFLOAT3>	m_normals;
	std::vector<DirectX::XMFLOAT2>	m_texCoords;

	DirectX::XMFLOAT3 m_ambient;
	DirectX::XMFLOAT3 m_diffuse;
	DirectX::XMFLOAT3 m_specular;
	
	float m_specularCoefficient;
	float m_transparency;
	float m_indexOfRefraction;

	std::wstring m_ambientTexName;
	std::wstring m_diffuseTexName;
	std::wstring m_specularTexName;
	std::wstring m_specularHighlightTexName;
	std::wstring m_alphaTexName;
	std::wstring m_bumpMapName;
	std::wstring m_displacementMapName;
	std::wstring m_stencilDecalMapName;

};

#endif //RAYTRACER_OBJ_LOADER_H