#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifndef STB_IMAGE_IMPLEMENTATION
	#define STB_IMAGE_IMPLEMENTATION
	#include "stb/stb_image.h"
#endif

#include "camera.h"
#include "material.h"
#include "entity.h"
#include "map.h"
#include "utilities.h"


#define MPYRAMID1 0
#define MPYRAMID2 1
#define MPYRAMID3 2
#define MSHIP 3
//#define MTEMPLE1 3
#define MTEMPLE2 40
#define MTEMPLE3 5
#define MOBELISK 6
#define MHOUSES1 7
#define MHOUSES2 8
#define MHOUSES3 9
#define MWOODENBOX 4

#define SHADER_LIGHTING 0
#define SHADER_SKYBOX   1

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


class Scene
{
public:
	Scene();
	 
	void Init();
	void Render();

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
	std::vector<Shader> m_Shaders;
	Model m_Skybox;
	std::shared_ptr<Map> m_Map;
	bool m_Night;
	float m_Time;
	float m_InitialTime;
	float m_DeltaTime;


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

	void drawSkybox(float time)
	{
		glDepthMask(GL_FALSE);
		m_Shaders[SHADER_SKYBOX].use();
		glm::mat4 proj = m_MainCamera->GetProjectionMatrix();
		glm::mat4 view = glm::mat4(glm::mat3(m_MainCamera->GetViewMatrix()));
		m_Shaders[SHADER_SKYBOX].setMat4("projection", proj);
		m_Shaders[SHADER_SKYBOX].setMat4("view", view);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureNight);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureDay);
		m_Skybox.DrawArrays();
		glDepthMask(GL_TRUE);
	}

	void createMap(Material material)
	{
		int width, height, nrComponents;
		unsigned char* data = stbi_load("resources/maps/map_big.png", &width, &height, &nrComponents, 1);

		m_Map = std::make_shared<Map>(data, width, height, width, height, material);
	}

	void drawMap()
	{
		glm::mat4 model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0, -111.0, 0));
		model = glm::scale(model, glm::vec3(1.5, 1, 1.5));

		m_Shaders[SHADER_LIGHTING].setMat4("model", model);
		m_Shaders[SHADER_LIGHTING].setFloat("material.shininess", m_Map->GetMaterial().Shininess);
		m_Shaders[SHADER_LIGHTING].setVec3("material.ambient", m_Map->GetMaterial().AmbientColor);
		m_Shaders[SHADER_LIGHTING].setVec3("material.diffuse", m_Map->GetMaterial().DiffuseColor);
		m_Shaders[SHADER_LIGHTING].setVec3("material.specular", m_Map->GetMaterial().SpecularColor);
		
		m_Map->Draw(m_Shaders[SHADER_LIGHTING]);

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
		front.x = cos(glm::radians(lastZ)) * cos(glm::radians(0.0));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(lastZ)) * cos(glm::radians(0.0));*/
	}

	void processDayLight()
	{

		float sinValue = glm::sin(glm::radians(m_InitialTime + m_Time * 10));
		float cosValue = glm::cos(glm::radians(m_InitialTime + m_Time * 10));

#if 0
		std::cout << "Time = " << m_Time << std::endl;
		std::cout << "sinValue = " << sinValue << std::endl;
#endif

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

		
#if 0
		std::cout << "Night = " << m_Night << std::endl;
		std::cout << "---------" << std::endl;
#endif

		m_Shaders[SHADER_SKYBOX].use();
		if (sinValue < 0)
			m_Shaders[SHADER_SKYBOX].setFloat("time", 0.0);
		else
			m_Shaders[SHADER_SKYBOX].setFloat("time", abs(sinValue));


		m_LightDir = glm::vec3(cosValue, -sinValue, -0.1);
	}

	void setUpShaders()
	{
		m_Shaders.emplace_back("vertex.glsl", "fragment.glsl");
		m_Shaders.emplace_back("skybox_vertex.glsl", "skybox_fragment.glsl");

		m_Shaders[SHADER_LIGHTING].use();
		m_Shaders[SHADER_LIGHTING].setBool("night", m_Night);

		m_Shaders[SHADER_LIGHTING].setInt("material.texture_diffuse1", 0);
		m_Shaders[SHADER_LIGHTING].setInt("material.texture_specular1", 1);
		
		m_Shaders[SHADER_LIGHTING].setVec3("dirLight.color", m_SunColor);

		m_Shaders[SHADER_LIGHTING].setVec3("spotlight.color", glm::vec3(1.0));
		m_Shaders[SHADER_LIGHTING].setFloat("spotlight.constant", 1.0);
		m_Shaders[SHADER_LIGHTING].setFloat("spotlight.linear", 0.007);
		m_Shaders[SHADER_LIGHTING].setFloat("spotlight.quadratic", 0.0002);
		m_Shaders[SHADER_LIGHTING].setFloat("spotlight.cutOff", glm::cos(glm::radians(12.5f)));
		m_Shaders[SHADER_LIGHTING].setFloat("spotlight.outerCutOff", glm::cos(glm::radians(17.5f)));

		m_Shaders[SHADER_LIGHTING].setInt("nPointLights", 3);

		m_Shaders[SHADER_LIGHTING].setVec3("pointLight[0].color", glm::vec3(1.0));
		m_Shaders[SHADER_LIGHTING].setFloat("pointLight[0].constant", 1.0);
		m_Shaders[SHADER_LIGHTING].setFloat("pointLight[0].linear", 0.007);
		m_Shaders[SHADER_LIGHTING].setFloat("pointLight[0].quadratic", 0.0002);

		m_Shaders[SHADER_LIGHTING].setVec3("pointLight[1].color", glm::vec3(1.0));
		m_Shaders[SHADER_LIGHTING].setFloat("pointLight[1].constant", 1.0);
		m_Shaders[SHADER_LIGHTING].setFloat("pointLight[1].linear", 0.007);
		m_Shaders[SHADER_LIGHTING].setFloat("pointLight[1].quadratic", 0.0002);

		m_Shaders[SHADER_LIGHTING].setVec3("pointLight[2].color", glm::vec3(1.0));
		m_Shaders[SHADER_LIGHTING].setFloat("pointLight[2].constant", 1.0);
		m_Shaders[SHADER_LIGHTING].setFloat("pointLight[2].linear", 0.007);
		m_Shaders[SHADER_LIGHTING].setFloat("pointLight[2].quadratic", 0.0002);

		m_Shaders[SHADER_LIGHTING].setBool("flashlight", true);

		
		m_Shaders[SHADER_SKYBOX].use();
		m_Shaders[SHADER_SKYBOX].setInt("skyboxDay", 0);
		m_Shaders[SHADER_SKYBOX].setInt("skyboxNight", 1);
	}

	void bindScene()
	{
		glm::mat4 proj = m_MainCamera->GetProjectionMatrix();
		glm::mat4 view = m_MainCamera->GetViewMatrix();

		m_Shaders[SHADER_LIGHTING].setMat4("projection", proj);
		m_Shaders[SHADER_LIGHTING].setMat4("view", view);
		m_Shaders[SHADER_LIGHTING].setVec3("SunPositon", m_SunPosition);
		m_Shaders[SHADER_LIGHTING].setVec3("viewerPos", m_MainCamera->Position);
		m_Shaders[SHADER_LIGHTING].setBool("night", m_Night);
		m_Shaders[SHADER_LIGHTING].setVec3("dirLight.color", m_SunColor);
		m_Shaders[SHADER_LIGHTING].setVec3("dirLight.dir", m_LightDir);
		m_Shaders[SHADER_LIGHTING].setVec3("spotlight.position", m_MainCamera->Position);
		m_Shaders[SHADER_LIGHTING].setVec3("spotlight.dir", m_MainCamera->Front);
		m_Shaders[SHADER_LIGHTING].setVec3("pointLight[0].position", pointLightPositions[0]);
		m_Shaders[SHADER_LIGHTING].setVec3("pointLight[1].position", pointLightPositions[1]);
		m_Shaders[SHADER_LIGHTING].setVec3("pointLight[2].position", pointLightPositions[2]);
	}

	void uploadModels()
	{
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

		Model woodenBox;
		Texture tex_diffuse;
		tex_diffuse.id = loadTexture("resources/textures/woodenbox_diffuse.png");
		tex_diffuse.type = "texture_diffuse";
		Texture tex_specular;
		tex_specular.id = loadTexture("resources/textures/woodenbox_specular.png");
		tex_specular.type = "texture_specular";
		std::vector<Texture> textures;
		textures.push_back(tex_diffuse);
		textures.push_back(tex_specular);
		woodenBox.CommitGeometry(woodenBoxVerts, woodenboxIndices, textures);
		m_Models.push_back(woodenBox);
	}

	void setPointLights()
	{
		pointLightPositions.push_back(glm::vec3(-150.0, 50.0, -100.0));
		pointLightPositions.push_back(glm::vec3(50.0, 26.0, -10.0));
		pointLightPositions.push_back(glm::vec3(-70.0, 10.0, -400.0));
	}

};