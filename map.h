#pragma once

#include <glm/gtx/matrix_transform_2d.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "model.h"
#include "material.h"
#include "stb.h"



class Map
{
public:
	Map(unsigned char* img, int hfWidth, int hfHeight, int width, int height, Material material);
	std::vector<Vertex> ComposeVertices() const;
	std::vector<unsigned int> ComposeIndices();
	void Draw(const Shader& shader);
	Vertex GetVertexAt(float x, float z);
	Material& GetMaterial();
	glm::ivec2 GetDim() const;


private:
	float getHeightfieldAt(int index) const;
	std::vector<Texture> uploadTextures();
	void createRiver();
	void drawRiver(const Shader& shader);


private:

	struct Heightfield
	{
		Heightfield() = default;
		Heightfield(unsigned char* img, int width, int height);
		~Heightfield();
		void Clear();

		unsigned char* m_Data = nullptr;
		glm::ivec2 m_HfDim;
	};

	Heightfield m_Heightfield;
	glm::ivec2 m_MapDim;
	glm::vec2 m_Origin;
	Model m_MapModel;
	Material m_Material;
	Model m_River;
};

