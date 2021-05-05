#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

#include "shader.h"


typedef unsigned int VertexBitfield;

#define NORMALS_BIT 0x1
#define TEXCOORDS_BIT 0x2

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

struct Texture
{
	unsigned int id;
	std::string type;
	std::string path;
};


class Mesh
{
public:
	std::vector<Vertex> m_Vertices;
	std::vector<float> m_VerticesFloat;
	std::vector<unsigned int> m_Indices;
	std::vector<Texture> m_Textures;

	Mesh(const std::vector<float>& verts, VertexBitfield mask);
	Mesh(const std::vector<float>& verts, const std::vector<unsigned int>& indices, VertexBitfield mask);
	Mesh(const std::vector<float>& verts, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures);
	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures);

	void DrawElements(const Shader &shader) const;
	void DrawElements() const;
	void DrawArrays() const;
	void DrawArrays(const Shader& shader) const;
	Vertex GetVertexAt(int idx) const
	{
		return m_Vertices[idx];
	}
	void SetTextures(const std::vector<Texture>& textures)
	{
		m_Textures = textures;
	}

private:
	unsigned int VAO, VBO, EBO;

	void setupMesh();
};
