#pragma once

#include <omp.h>

#include "model.h"
#include "camera.h"
#include "material.h"
#include "entity.h"
#include "utilities.h"


#define MPYRAMID1 0
#define MPYRAMID2 1
#define MPYRAMID3 2
#define MSHIP 3
//#define MTEMPLE1 3
#define MTEMPLE2 4
#define MTEMPLE3 5
#define MOBELISK 6
#define MHOUSES1 7
#define MHOUSES2 8
#define MHOUSES3 9

static unsigned int loadCubemapTexture(std::vector<std::string> faces)
{
	unsigned int texId;
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texId);

	int width, height, nrChannels;
	for (int i = 0; i < faces.size(); i++)
	{
		unsigned char * data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return texId;
}


struct MeshGeometry
{
 	//todo
};


class Map
{
public:
	Map(unsigned char* img, int hfWidth, int hfHeight, int width, int height, Material material)
		: m_Heightfield(img, hfWidth, hfHeight), m_MapDim(glm::ivec2(width, height)), m_HeightPrcnt(1.0), m_Origin(-512.0, -768.0)
	{
		std::vector<Vertex> vertices = ComposeVertices();
		std::vector<unsigned int> indices = ComposeIndices();
		std::vector<Texture> textures = uploadTextures();
		m_MapModel.CommitGeometry(vertices, indices, textures);

		m_Material = material;
	}

	//void UpdateHeightfield(unsigned char* img, int hfWidth, int hfHeight)
	//{
	//	m_Heightfield.Clear();
	//	m_Heightfield.Create(img, hfWidth, hfHeight);
	//}

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
					r = glm::vec3((x+1) - x, getHeightfieldAt(z * m_MapDim.x + (x + 1)) - y, z - z);
				if (z - 1 >= 0)
					u = glm::vec3(x - x, y - getHeightfieldAt((z - 1) * m_MapDim.x + x), z - (z - 1));
				if (z + 1 < m_MapDim.y)
					d = glm::vec3(x-x, getHeightfieldAt((z + 1) * m_MapDim.x + x) - y, (z+1) - z);

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

	void Draw(const Shader & shader)
	{
		m_MapModel.DrawElements(shader);
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
			if(m_Data)
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

private:
	float getHeightfieldAt(int index) const
	{
		return (float)m_Heightfield.m_Data[index];
	}

	std::vector<Texture> uploadTextures()
	{
		Texture t;
		t.id = loadTexture("sand.jpg");
		t.type = "texture_diffuse";
		std::vector<Texture> textures;
		textures.push_back(t);
		return textures;
	}
};

class Scene
{
public:
	Scene()
	{
		//Cameras
		m_Cameras.emplace_back(glm::vec3(0.0, 5.0, 3.0), -90.0, 0.0);
		m_Cameras.emplace_back(glm::vec3(-10.0, 5.0, 3.0), -90.0, 0.0);
		m_Cameras.emplace_back(glm::vec3(10.0, 20.0, 3.0), -90.0, 0.0);
		m_Cameras.emplace_back(glm::vec3(10.0, 20.0, 3.0), -90.0, 0.0);
		m_MainCamera = &m_Cameras[0];

		m_SunPosition = glm::vec3(10.0f);
		m_SunColor = glm::vec3(1.0f);
		m_LightDir = glm::vec3(-0.5f, -0.9, 0.8);

		m_InitialTime = 230.0;
		m_DeltaTime = 10.0;

	}
	 
	void Init()
	{
		int i = 0;

		pointLightPositions.push_back(glm::vec3(-150.0, 50.0, -100.0));
		pointLightPositions.push_back(glm::vec3(50.0, 26.0, -10.0));
		pointLightPositions.push_back(glm::vec3(-70.0, 10.0, -400.0));

		m_LightingShader = std::make_shared<Shader>("vertex.glsl", "fragment.glsl");
		m_LightingShader->use();
		m_LightingShader->setBool("night", m_Night);
		

		m_LightingShader->setInt("material.texture_diffuse1", 0);
		m_LightingShader->setInt("material.texture_specular1", 1);
		
		m_LightingShader->setFloat("spotlight.constant", 1.0);
		m_LightingShader->setFloat("spotlight.linear", 0.007);
		m_LightingShader->setFloat("spotlight.quadratic", 0.0002);
		
		m_LightingShader->setInt("nPointLights", 3);
		m_LightingShader->setFloat("pointLight[0].constant", 1.0);
		m_LightingShader->setFloat("pointLight[0].linear", 0.007);
		m_LightingShader->setFloat("pointLight[0].quadratic", 0.0002);

		m_LightingShader->setFloat("pointLight[1].constant", 1.0);
		m_LightingShader->setFloat("pointLight[1].linear", 0.007);
		m_LightingShader->setFloat("pointLight[1].quadratic", 0.0002);

		m_LightingShader->setFloat("pointLight[2].constant", 1.0);
		m_LightingShader->setFloat("pointLight[2].linear", 0.007);
		m_LightingShader->setFloat("pointLight[2].quadratic", 0.0002);
		
		m_LightingShader->setBool("flashlight", true);



		m_SkyboxShader = std::make_shared<Shader>("skybox_vertex.glsl", "skybox_fragment.glsl");
		m_SkyboxShader->use();
		m_SkyboxShader->setInt("skyboxDay", 0);
		m_SkyboxShader->setInt("skyboxNight", 1);
		createSkybox();


		

		m_MapShader = std::make_shared<Shader>("map_vertex.glsl", "fragment.glsl");
		m_MapShader->use();
		m_MapShader->setInt("material.texture_diffuse1", 0);
		m_MapShader->setInt("material.texture_specular1", 1);

		m_MapShader->setFloat("spotlight.constant", 1.0);
		m_MapShader->setFloat("spotlight.linear", 0.007);
		m_MapShader->setFloat("spotlight.quadratic", 0.0002);

		m_MapShader->setInt("nPointLights", 3);
		m_MapShader->setFloat("pointLight[0].constant", 1.0);
		m_MapShader->setFloat("pointLight[0].linear", 0.007);
		m_MapShader->setFloat("pointLight[0].quadratic", 0.0002);

		m_MapShader->setFloat("pointLight[1].constant", 1.0);
		m_MapShader->setFloat("pointLight[1].linear", 0.007);
		m_MapShader->setFloat("pointLight[1].quadratic", 0.0002);

		m_MapShader->setFloat("pointLight[2].constant", 1.0);
		m_MapShader->setFloat("pointLight[2].linear", 0.007);
		m_MapShader->setFloat("pointLight[2].quadratic", 0.0002);

		m_MapShader->setBool("flashlight", true);


		Material material(glm::vec3(0.2), glm::vec3(0.5), glm::vec3(1.0), 32.0);
		createMap(material);
		createRiver();



		m_Models.emplace_back("resources/models/egypt/pyramid1.obj");
		m_Models.emplace_back("resources/models/egypt/pyramid2.obj");
		m_Models.emplace_back("resources/models/egypt/pyramid3.obj");
		m_Models.emplace_back("resources/models/egypt/ship.obj");
		//m_Models.emplace_back("resources/models/egypt/temple1.obj");
		//m_Models.emplace_back("resources/models/egypt/temple2.obj");
		//m_Models.emplace_back("resources/models/egypt/temple3.obj");
		//m_Models.emplace_back();
		//m_Models.emplace_back();
		//m_Models.emplace_back();
		//m_Models.emplace_back();
		//m_Models.emplace_back("resources/models/egypt/obelisk.obj");
		//m_Models.emplace_back("resources/models/egypt/houses1.obj");
		//m_Models.emplace_back("resources/models/egypt/houses2.obj");
		//m_Models.emplace_back("resources/models/egypt/houses3.obj");


		m_Entities.emplace_back(&m_Models[MPYRAMID1], material, "lighting", glm::vec3(0.0f, 0.0f, -310.0f), glm::vec3(8.0));
		m_Entities.emplace_back(&m_Models[MPYRAMID2], material, "lighting", glm::vec3(300.0f, 0.0, -280.0), glm::vec3(8.0));
		m_Entities.emplace_back(&m_Models[MPYRAMID3], material, "lighting", glm::vec3(-300.0f, 0.0, -180.0), glm::vec3(8.0));
		m_Entities.emplace_back(&m_Models[MSHIP], material, "lighting", glm::vec3(0.0f, -3.5, 130.0), glm::vec3(0.2), glm::radians(90.0));
		m_Entities[m_Entities.size() - 1].SetDirection(glm::vec3(1.0, 0.0, 0.0));
		m_Entities[m_Entities.size() - 1].SetVelocity(0.5);
		m_Entities[m_Entities.size() - 1].AttachCamera(&m_Cameras[1], glm::vec3(2.0, 5.0, -1.0));


		

		//m_Entities.emplace_back(&m_Models[MTEMPLE1], material, "lighting", glm::vec3(-90.0f, 0.0, -140.0), glm::vec3(1.0), 180.0f);
		//m_Entities.emplace_back(&m_Models[MTEMPLE2], material, "lighting", glm::vec3(0.0f, 0.0, 0.0), glm::vec3(2.0));
		//m_Entities.emplace_back(&m_Models[MTEMPLE3], material, "lighting", glm::vec3(100.0f, 0.0, 0.0), glm::vec3(2.0));
		//m_Entities.emplace_back(&m_Models[MOBELISK], material, "lighting", glm::vec3(-90.0f, 0.01, -180.0), glm::vec3(2.0));
		//m_Entities.emplace_back(&m_Models[MHOUSES1], material, "lighting", glm::vec3(-100.0f, 3.0, 100.0), glm::vec3(1.0));
		//m_Entities.emplace_back(&m_Models[MHOUSES2], material, "lighting", glm::vec3(0.0f, 3.0, 100.0), glm::vec3(1.0));
		//m_Entities.emplace_back(&m_Models[MHOUSES3], material, "lighting", glm::vec3(-156.0f, 0.0, -180.0), glm::vec3(2.0));
		//m_Entities.emplace_back(&m_Models[MHOUSES3], material, "lighting", glm::vec3(-156.0f, 0.0, -140.0), glm::vec3(2.0));
		//m_Entities.emplace_back(&m_Models[MHOUSES3], material, "lighting", glm::vec3(-150.0f, 0.0, -100.0), glm::vec3(2.0));
		//m_Entities.emplace_back(&m_Models[MHOUSES3], material, "lighting", glm::vec3(-50.0f, 0.0, -100.0), glm::vec3(2.0));

	}

	void Render()
	{
		m_Time = glfwGetTime();
		float radius = 20;
		//m_LightDir = glm::vec3(cos(time),sin(time), m_LightDir.z);

		processDayLight();
		drawSkybox(m_Time);
		drawMap();

		glm::mat4 proj = m_MainCamera->GetProjectionMatrix();
		glm::mat4 view = m_MainCamera->GetViewMatrix();

		m_LightingShader->use();
		m_LightingShader->setMat4("projection", proj);
		m_LightingShader->setMat4("view", view);
		m_LightingShader->setVec3("SunColor", m_SunColor);
		m_LightingShader->setVec3("SunPositon", m_SunPosition);
		m_LightingShader->setBool("night", m_Night);

		drawRiver();

		for (auto& entity : m_Entities)
		{
			if (entity.GetShaderType() == "lighting")
			{
				//handleCollision(entity);
				if (entity.GetModel() == &m_Models[MSHIP])
					moveAlongEllise(entity);
				entity.Advance();
				glm::mat4 model = entity.GetModelMatrix();

				
				m_LightingShader->setMat4("model", model);
				m_LightingShader->setVec3("viewerPos", m_MainCamera->Position);
				m_LightingShader->setFloat("material.shininess", entity.GetMaterial().Shininess);
				m_LightingShader->setVec3("dirLight.dir", m_LightDir);
				m_LightingShader->setVec3("dirLight.ambient", entity.GetMaterial().AmbientColor);
				m_LightingShader->setVec3("dirLight.diffuse", entity.GetMaterial().DiffuseColor * m_SunColor);
				m_LightingShader->setVec3("dirLight.specular", entity.GetMaterial().SpecularColor * m_SunColor);

				m_LightingShader->setVec3("spotlight.position", m_MainCamera->Position);
				m_LightingShader->setVec3("spotlight.dir", m_MainCamera->Front);
				m_LightingShader->setVec3("spotlight.ambient", entity.GetMaterial().AmbientColor * glm::vec3(1.0));
				m_LightingShader->setVec3("spotlight.diffuse", entity.GetMaterial().DiffuseColor * glm::vec3(1.0));
				m_LightingShader->setVec3("spotlight.specular", entity.GetMaterial().SpecularColor * glm::vec3(1.0));
				m_LightingShader->setFloat("spotlight.cutOff", glm::cos(glm::radians(12.5f)));
				m_LightingShader->setFloat("spotlight.outerCutOff", glm::cos(glm::radians(17.5f)));

				m_LightingShader->setVec3("pointLight[0].position", pointLightPositions[0]);
				m_LightingShader->setVec3("pointLight[0].ambient", entity.GetMaterial().AmbientColor * glm::vec3(1.0));
				m_LightingShader->setVec3("pointLight[0].diffuse", entity.GetMaterial().DiffuseColor * glm::vec3(1.0));
				m_LightingShader->setVec3("pointLight[0].specular", entity.GetMaterial().SpecularColor * glm::vec3(1.0));
				
				m_LightingShader->setVec3("pointLight[1].position", pointLightPositions[1]);
				m_LightingShader->setVec3("pointLight[1].ambient", entity.GetMaterial().AmbientColor * glm::vec3(1.0));
				m_LightingShader->setVec3("pointLight[1].diffuse", entity.GetMaterial().DiffuseColor * glm::vec3(1.0));
				m_LightingShader->setVec3("pointLight[1].specular", entity.GetMaterial().SpecularColor * glm::vec3(1.0));

				m_LightingShader->setVec3("pointLight[2].position", pointLightPositions[2]);
				m_LightingShader->setVec3("pointLight[2].ambient", entity.GetMaterial().AmbientColor * glm::vec3(1.0));
				m_LightingShader->setVec3("pointLight[2].diffuse", entity.GetMaterial().DiffuseColor * glm::vec3(1.0));
				m_LightingShader->setVec3("pointLight[2].specular", entity.GetMaterial().SpecularColor * glm::vec3(1.0));

				//m_LightingShader->setVec3("light.position", m_MainCamera->Position);
				//m_LightingShader->setVec3("light.dir", m_MainCamera->Front);
				entity.Draw(*m_LightingShader);
			}
		}

	}

	void SetMainCamera(int id)
	{
		m_MainCamera = &m_Cameras[id];
	}

	Camera* GetCamera()
	{
		return m_MainCamera;
	}

private:
	std::vector<Model> m_Models;
	std::vector<Entity> m_Entities;
	std::vector<Camera> m_Cameras;
	Camera * m_MainCamera;
	glm::vec3 m_SunPosition;
	glm::vec3 m_SunColor;
	glm::vec3 m_LightDir;
	std::shared_ptr<Shader> m_LightingShader;
	std::shared_ptr<Shader> m_SkyboxShader;
	std::shared_ptr<Shader> m_MapShader;
	std::vector<std::shared_ptr<Shader>> m_Shaders;
	Model m_Skybox;
	std::shared_ptr<Map> m_Map;
	Model m_River;
	bool m_Night;
	float m_Time;
	float m_InitialTime;
	float m_DeltaTime;

	//tmp
	unsigned int cubeMapTextureDay;
	unsigned int cubeMapTextureNight;
	std::vector<glm::vec3> pointLightPositions;

private:
	void createSkybox()
	{
		std::vector<float> skyboxVerts = {
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};


		std::vector<std::string> facesNight
		{
			"resources/textures/skybox/right_night.jpg",
			"resources/textures/skybox/left_night.jpg",
			"resources/textures/skybox/top_night.jpg",
			"resources/textures/skybox/bottom_night.jpg",
			"resources/textures/skybox/front_night.jpg",
			"resources/textures/skybox/back_night.jpg",
		};

		std::vector<std::string> facesDay
		{
			"resources/textures/skybox/right.jpg",
			"resources/textures/skybox/left.jpg",
			"resources/textures/skybox/top.jpg",
			"resources/textures/skybox/bottom.jpg",
			"resources/textures/skybox/front.jpg",
			"resources/textures/skybox/back.jpg",
		};

		m_Skybox.CommitGeometry(skyboxVerts);

		cubeMapTextureDay = loadCubemapTexture(facesDay);
		cubeMapTextureNight = loadCubemapTexture(facesNight);
	}

	void drawSkybox(float time) const
	{
		glDepthMask(GL_FALSE);
		m_SkyboxShader->use();
		glm::mat4 proj = m_MainCamera->GetProjectionMatrix();
		glm::mat4 view = glm::mat4(glm::mat3(m_MainCamera->GetViewMatrix()));
		m_SkyboxShader->setMat4("projection", proj);
		m_SkyboxShader->setMat4("view", view);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureNight);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureDay);
		m_Skybox.DrawArrays();
		glDepthMask(GL_TRUE);
	}

	//heightfield
	void createMap(Material material)
	{
		int width, height, nrComponents;
		unsigned char* data = stbi_load("resources/maps/map_big.png", &width, &height, &nrComponents, 1);

		m_Map = std::make_shared<Map>(data, width, height, width, height, material);
	}

	void drawMap()
	{
		glm::mat4 proj = m_MainCamera->GetProjectionMatrix();
		glm::mat4 view = m_MainCamera->GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0, -111.0, 0));
		model = glm::scale(model, glm::vec3(1.5, 1, 1.5));

		m_MapShader->use();
		m_MapShader->setBool("night", m_Night);
		m_MapShader->setMat4("projection", proj);
		m_MapShader->setMat4("view", view);
		m_MapShader->setMat4("model", model);
		m_MapShader->setVec3("SunColor", m_SunColor);
		m_MapShader->setVec3("SunPositon", m_SunPosition);
		m_MapShader->setVec3("viewerPos", m_MainCamera->Position);
		m_MapShader->setFloat("material.shininess", m_Map->GetMaterial().Shininess);

		m_MapShader->setVec3("dirLight.dir", m_LightDir);
		m_MapShader->setVec3("dirLight.ambient", m_Map->GetMaterial().AmbientColor * m_SunColor);
		m_MapShader->setVec3("dirLight.diffuse", m_Map->GetMaterial().DiffuseColor * m_SunColor);
		m_MapShader->setVec3("dirLight.specular", m_Map->GetMaterial().SpecularColor * m_SunColor);

		m_MapShader->setVec3("spotlight.position", m_MainCamera->Position);
		m_MapShader->setVec3("spotlight.dir", m_MainCamera->Front);
		m_MapShader->setVec3("spotlight.ambient", m_Map->GetMaterial().AmbientColor * glm::vec3(1.0));
		m_MapShader->setVec3("spotlight.diffuse", m_Map->GetMaterial().DiffuseColor * glm::vec3(1.0));
		m_MapShader->setVec3("spotlight.specular", m_Map->GetMaterial().SpecularColor * glm::vec3(1.0));
		m_MapShader->setFloat("spotlight.cutOff", glm::cos(glm::radians(12.5f)));
		m_MapShader->setFloat("spotlight.outerCutOff", glm::cos(glm::radians(17.5f)));

		m_MapShader->setVec3("pointLight[0].position", pointLightPositions[0]);
		m_MapShader->setVec3("pointLight[0].ambient", m_Map->GetMaterial().AmbientColor * glm::vec3(1.0));
		m_MapShader->setVec3("pointLight[0].diffuse", m_Map->GetMaterial().DiffuseColor * glm::vec3(1.0));
		m_MapShader->setVec3("pointLight[0].specular", m_Map->GetMaterial().SpecularColor * glm::vec3(1.0));

		m_MapShader->setVec3("pointLight[1].position", pointLightPositions[1]);
		m_MapShader->setVec3("pointLight[1].ambient", m_Map->GetMaterial().AmbientColor * glm::vec3(1.0));
		m_MapShader->setVec3("pointLight[1].diffuse", m_Map->GetMaterial().DiffuseColor * glm::vec3(1.0));
		m_MapShader->setVec3("pointLight[1].specular", m_Map->GetMaterial().SpecularColor * glm::vec3(1.0));

		m_MapShader->setVec3("pointLight[2].position", pointLightPositions[2]);
		m_MapShader->setVec3("pointLight[2].ambient", m_Map->GetMaterial().AmbientColor * glm::vec3(1.0));
		m_MapShader->setVec3("pointLight[2].diffuse", m_Map->GetMaterial().DiffuseColor * glm::vec3(1.0));
		m_MapShader->setVec3("pointLight[2].specular", m_Map->GetMaterial().SpecularColor * glm::vec3(1.0));

		m_Map->Draw(*m_MapShader);

	}

	void handleCollision(Entity & entity)
	{
		glm::vec3 epos = entity.GetPosition();
		Vertex map_vert = m_Map->GetVertexAt(epos.x, epos.z);
		glm::vec3 up(0.0, 1.0, 0.0);
		glm::vec3 axis = glm::cross(up, map_vert.normal);
		float dot = glm::dot(up, map_vert.normal);
		float angle = std::acos(dot);

		epos.y = map_vert.position.y;

		entity.SetPosition(epos);
		if(axis != glm::vec3(0.0))
			entity.Rotate(angle, axis);
	}

	void createRiver()
	{
		int riverWidth = 125;
		int riverLength = m_Map->GetDim().x / 2;

		std::vector<Vertex> vertices;

		for(int z = 0; z < riverWidth; z++)
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
					indices.push_back(j * riverLength  + i);
					indices.push_back(j * riverLength + (i - 1));
					indices.push_back((j - 1) * riverLength + i);
				}
			}

		//textures
		Texture t;
		t.id = loadTexture("resources/textures/water1.jpg");
		t.type = "texture_diffuse";
		std::vector<Texture> textures;
		textures.push_back(t);

		m_River.CommitGeometry(vertices, indices, textures);
	}

	void drawRiver()
	{
		int map_width = m_Map->GetDim().x;

		glm::mat4 model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-map_width/2*1.5, -2.0, 10.0));
		model = glm::scale(model, glm::vec3(3.0, 0.0, 3.0));
		m_LightingShader->setMat4("model", model);
		m_LightingShader->setBool("river", true);
		m_LightingShader->setFloat("time", glfwGetTime());


		m_River.DrawElements(*m_LightingShader);
		m_LightingShader->setBool("river", false);
	}

	void moveAlongEllise(Entity & entity)
	{
		float time = glfwGetTime();
		float a = 500, b = 50, h = 0, k = 180;
		glm::vec3 pos = entity.GetPosition();
		float lastZ = pos.z;
		pos = glm::vec3(h + a * cos(time/40), pos.y, k + b * sin(time/40));
		entity.SetPosition(pos);
		
		/*float offsetZ = pos.z - lastZ;
		float angle = entity.GetRotationAngle();
		angle += abs(offsetZ);
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));*/
	}

	void processDayLight()
	{

		float sinValue = glm::sin(glm::radians(m_InitialTime + m_Time * 10));
		float cosValue = glm::cos(glm::radians(m_InitialTime + m_Time * 10));

		std::cout << "Time = " << m_Time << std::endl;
		std::cout << "sinValue = " << sinValue << std::endl;
		if (sinValue <= 0.5)
		{
			m_Night = true;
		}
		else
		{
			m_Night = false;
		}

		if (sinValue <= 0.1)
		{
			m_SunColor = glm::vec3(0.1);
		}
		else
		{
			m_SunColor = glm::vec3(sinValue);
		}

		

		std::cout << "Night = " << m_Night << std::endl;
		std::cout << "---------" << std::endl;

		m_SkyboxShader->use();
		if (sinValue < 0)
			m_SkyboxShader->setFloat("time", 0.0);
		else
			m_SkyboxShader->setFloat("time", abs(sinValue));


		m_LightDir = glm::vec3(cosValue, -sinValue, -0.1);
	}

};