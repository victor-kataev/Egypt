#pragma once

#include "model.h"
#include "material.h"


static unsigned int loadTexture(const char* texPath)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(texPath, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << texPath << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}


class Map
{
public:
	Map(unsigned char* img, int hfWidth, int hfHeight, int width, int height, Material material)
		: m_Heightfield(img, hfWidth, hfHeight), m_MapDim(glm::ivec2(width, height)), m_HeightPrcnt(1.0), m_Origin(-512.0, -768.0), m_Material(material)
	{
		std::vector<Vertex> vertices = ComposeVertices();
		std::vector<unsigned int> indices = ComposeIndices();
		std::vector<Texture> textures = uploadTextures();
		m_MapModel.CommitGeometry(vertices, indices, textures);
		createRiver();
	}

	std::vector<Vertex> ComposeVertices() const
	{
		std::vector<Vertex> vertices;

		for (int z = 0; z < m_MapDim.y; z++)
			for (int x = 0; x < m_MapDim.x; x++)
			{
				Vertex v;

				//position
				float y = getHeightfieldAt(z * m_MapDim.x + x);
				v.position.x = x + m_Origin.x;
				v.position.y = y;
				v.position.z = z + m_Origin.y;

				//texcoords
				if (x % 2 == 0)
					if (z % 2 == 0)
						v.texCoords = glm::vec2(0.0, 0.0);
					else
						v.texCoords = glm::vec2(0.0, 1.0);
				else
					if (z % 2 == 0)
						v.texCoords = glm::vec2(1.0, 0.0);
					else
						v.texCoords = glm::vec2(1.0, 1.0);

				//normals
				glm::vec3 u(0.0);
				glm::vec3 d(0.0);
				glm::vec3 r(0.0);
				glm::vec3 l(0.0);

				if (x - 1 >= 0)
					l = glm::vec3(x - (x - 1), y - getHeightfieldAt(z * m_MapDim.x + (x - 1)), z - z);
				if (x + 1 < m_MapDim.x)
					r = glm::vec3((x + 1) - x, getHeightfieldAt(z * m_MapDim.x + (x + 1)) - y, z - z);
				if (z - 1 >= 0)
					u = glm::vec3(x - x, y - getHeightfieldAt((z - 1) * m_MapDim.x + x), z - (z - 1));
				if (z + 1 < m_MapDim.y)
					d = glm::vec3(x - x, getHeightfieldAt((z + 1) * m_MapDim.x + x) - y, (z + 1) - z);

				glm::vec3 normal = glm::normalize(glm::cross(u, l) + glm::cross(u, r) + glm::cross(d, l) + glm::cross(d, r));
				v.normal = normal;

				vertices.push_back(v);
			}
		return vertices;
	}

	std::vector<unsigned int> ComposeIndices()
	{
		std::vector<unsigned int> indices;

		for (int z = 0, j = m_MapDim.y - 1; z < m_MapDim.y && j >= 0; z++, j--)
			for (int x = 0, i = m_MapDim.x - 1; x < m_MapDim.x && i >= 0; x++, i--)
			{
				if (x + 1 < m_MapDim.x && z + 1 < m_MapDim.y)
				{
					indices.push_back(z * m_MapDim.x + x); //position in vertex buffer
					indices.push_back(z * m_MapDim.x + (x + 1));
					indices.push_back((z + 1) * m_MapDim.x + x);
				}
				if (j - 1 >= 0 && i - 1 >= 0)
				{
					indices.push_back(j * m_MapDim.x + i);
					indices.push_back(j * m_MapDim.x + (i - 1));
					indices.push_back((j - 1) * m_MapDim.x + i);
				}
			}
		return indices;
	}

	void Draw(const Shader& shader)
	{
		m_MapModel.DrawElements(shader);
		drawRiver(shader);
	}

	Vertex GetVertexAt(float x, float z)
	{
		glm::vec2 map_local = glm::vec2(x, z) + abs(m_Origin);

		return m_MapModel.GetVertexOfMeshAt(0, map_local.y * m_MapDim.x + map_local.x);
	}

	Material& GetMaterial()
	{
		return m_Material;
	}

	glm::ivec2 GetDim() const
	{
		return m_MapDim;
	}

private:

	struct Heightfield
	{
		Heightfield() = default;

		Heightfield(unsigned char* img, int width, int height)
			: m_HfDim(glm::ivec2(width, height))
		{
			m_Data = new unsigned char[width * height];
			memcpy(m_Data, img, width * height);
		}

		~Heightfield()
		{
			if (m_Data)
				delete[] m_Data;
		}

		//void Create(unsigned char* img, int width, int height)
		//{
		//	Heightfield(img, width, height);
		//}

		void Clear()
		{
			m_HfDim = glm::ivec2(0.0);
			if (m_Data)
				delete[] m_Data;
		}

		unsigned char* m_Data = nullptr;
		glm::ivec2 m_HfDim;
	};

	Heightfield m_Heightfield;
	glm::ivec2 m_MapDim;
	float m_HeightPrcnt; // [0.0, 1.0]
	glm::vec2 m_Origin;
	Model m_MapModel;
	Material m_Material;
	float m_SeaLevel;
	Model m_River;

private:
	float getHeightfieldAt(int index) const
	{
		return (float)m_Heightfield.m_Data[index];
	}

	std::vector<Texture> uploadTextures()
	{
		Texture t;
		t.id = loadTexture("resources/textures/sand.jpg");
		t.type = "texture_diffuse";
		std::vector<Texture> textures;
		textures.push_back(t);
		return textures;
	}

	void createRiver()
	{
		int riverWidth = 80;
		int riverLength = m_MapDim.x / 2;

		std::vector<Vertex> vertices;

		for (int z = 0; z < riverWidth; z++)
			for (int x = 0; x < riverLength; x++)
			{
				Vertex v;

				//position
				v.position.x = x;
				v.position.y = 10;
				v.position.z = z;

				//texcoords
				if (x % 2 == 0)
					if (z % 2 == 0)
						v.texCoords = glm::vec2(0.0, 0.0);
					else
						v.texCoords = glm::vec2(0.0, 1.0);
				else
					if (z % 2 == 0)
						v.texCoords = glm::vec2(1.0, 0.0);
					else
						v.texCoords = glm::vec2(1.0, 1.0);

				//normals
				v.normal = glm::vec3(0.0, 1.0, 0.0);

				vertices.push_back(v);
			}

		//indices
		std::vector<unsigned int> indices;
		for (int z = 0, j = riverWidth - 1; z < riverWidth && j >= 0; z++, j--)
			for (int x = 0, i = riverLength - 1; x < riverLength && i >= 0; x++, i--)
			{
				if (x + 1 < riverLength && z + 1 < riverWidth)
				{
					indices.push_back(z * riverLength + x); //position in vertex buffer
					indices.push_back(z * riverLength + (x + 1));
					indices.push_back((z + 1) * riverLength + x);
				}
				if (j - 1 >= 0 && i - 1 >= 0)
				{
					indices.push_back(j * riverLength + i);
					indices.push_back(j * riverLength + (i - 1));
					indices.push_back((j - 1) * riverLength + i);
				}
			}

		//textures
		Texture t;
		t.id = loadTexture("resources/textures/water.jpg");
		t.type = "texture_diffuse";
		std::vector<Texture> textures;
		textures.push_back(t);

		m_River.CommitGeometry(vertices, indices, textures);
	}

	void drawRiver(const Shader& shader)
	{
		int map_width = m_MapDim.x;

		glm::mat4 model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-map_width / 2 * 1.5, -2.8, 150.0));
		model = glm::scale(model, glm::vec3(3.0, 0.0, 3.0));
		shader.setMat4("model", model);
		shader.setBool("river", true);
		shader.setFloat("time", glfwGetTime());

		m_River.DrawElements(shader);
		shader.setBool("river", false);
	}

};

