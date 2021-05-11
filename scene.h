#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifndef STB_IMAGE_IMPLEMENTATION
	#define STB_IMAGE_IMPLEMENTATION
	#include "stb_image.h"
#endif

#include "camera.h"
#include "material.h"
#include "entity.h"
#include "map.h"
#include "utilities.h"


#define PI 3.14159265359
#define E  2.71828182845

#define MPYRAMID1		0
#define MPYRAMID2		1
#define MPYRAMID3		2
#define MTEMPLE1		3
#define MTEMPLE2		4
#define MTEMPLE3		5
#define MOBELISK		6
#define MHOUSES3		7
#define MSHIP			8
#define MWOODENBOX		9
#define MTORCH			10
#define MMINIPYRAMIDS	11


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
	Scene()
	{
		m_Cameras.emplace_back(glm::vec3(0.0, 5.0, 3.0));
		m_Cameras.emplace_back(glm::vec3(-10.0, 5.0, 3.0));
		m_Cameras.emplace_back(glm::vec3(0.0, 100.0, -350.0), true);
		m_Cameras.emplace_back(glm::vec3(10.0, 20.0, 3.0));
		m_MainCamera = &m_Cameras[0];

		m_SunPosition = glm::vec3(10.0f);
		m_SunColor = glm::vec3(1.0f);
		m_LightDir = glm::vec3(-0.5f, -0.9, 0.8);

		m_InitialTime = 230.0;
		m_DeltaTime = 1.0;
		m_Flashlight = false;
	}
	 
	void Init()
	{
		createShaders();

		createSkybox();

		Material material(glm::vec3(0.2), glm::vec3(0.5), glm::vec3(1.0), 32.0);
		createMap(material);

		uploadModels();
		setPointLights();

		//entities
		m_Entities.emplace_back(&m_Models[MPYRAMID1], material, glm::vec3(0.0f, 0.0f, -310.0f), glm::vec3(8.0));
		m_Entities.emplace_back(&m_Models[MPYRAMID2], material, glm::vec3(300.0f, 0.0, -280.0), glm::vec3(8.0));
		m_Entities.emplace_back(&m_Models[MPYRAMID3], material, glm::vec3(-300.0f, 0.0, -180.0), glm::vec3(8.0));
		m_Entities.emplace_back(&m_Models[MSHIP], material, glm::vec3(0.0f, -5.0, 0.0), glm::vec3(0.2), glm::radians(0.0));
		m_Entities[m_Entities.size() - 1].SetDirection(glm::vec3(0.0, 0.0, 1.0));
		m_Entities[m_Entities.size() - 1].SetVelocity(5.5);
		m_Entities[m_Entities.size() - 1].AttachCamera(&m_Cameras[1], glm::vec3(2.0, 5.0, -1.0));

		m_Entities.emplace_back(&m_Models[MOBELISK], material, glm::vec3(-90.0f, 0.01, -180.0), glm::vec3(2.0));

		m_Entities.emplace_back(&m_Models[MHOUSES3], material, glm::vec3(-156.0f, 0.0, -180.0), glm::vec3(2.0));
		m_Entities.emplace_back(&m_Models[MHOUSES3], material, glm::vec3(-156.0f, 0.0, -220.0), glm::vec3(2.0));
		m_Entities.emplace_back(&m_Models[MHOUSES3], material, glm::vec3(-100.0f, 0.0, -220.0), glm::vec3(2.0));
		m_Entities.emplace_back(&m_Models[MHOUSES3], material, glm::vec3(-156.0f, 0.0, -140.0), glm::vec3(2.0));
		m_Entities.emplace_back(&m_Models[MHOUSES3], material, glm::vec3(-156.0f, 0.0, -100.0), glm::vec3(2.0));
		m_Entities.emplace_back(&m_Models[MTEMPLE1], material, glm::vec3(-90.0f, 0.0, -150.0), glm::vec3(1.0), glm::radians(180.0f));
		m_Entities.emplace_back(&m_Models[MHOUSES3], material, glm::vec3(-50.0f, 0.0, -100.0), glm::vec3(2.0));
		
		m_Entities.emplace_back(&m_Models[MHOUSES3], material, glm::vec3(-50.0f, 0.0, -60.0), glm::vec3(2.0));
		m_Entities.emplace_back(&m_Models[MHOUSES3], material, glm::vec3(-50.0f, 0.0, -20.0), glm::vec3(2.0));
		m_Entities.emplace_back(&m_Models[MHOUSES3], material, glm::vec3(-105.0f, 0.0, -60.0), glm::vec3(2.0));

		m_Entities.emplace_back(&m_Models[MTEMPLE3], material, glm::vec3(50.0f, 0.0, -200.0), glm::vec3(3.0));
		m_Entities.emplace_back(&m_Models[MOBELISK], material, glm::vec3(50.0f, 0.01, -160.0), glm::vec3(2.0));
		m_Entities.emplace_back(&m_Models[MHOUSES3], material, glm::vec3(100.0f, 0.0, -180.0), glm::vec3(2.0));
		m_Entities.emplace_back(&m_Models[MHOUSES3], material, glm::vec3(100.0f, 0.0, -140.0), glm::vec3(2.0));
		m_Entities.emplace_back(&m_Models[MHOUSES3], material, glm::vec3(100.0f, 0.0, -100.0), glm::vec3(2.0));
		m_Entities.emplace_back(&m_Models[MHOUSES3], material, glm::vec3(150.0f, 0.0, -100.0), glm::vec3(2.0));
		m_Entities.emplace_back(&m_Models[MHOUSES3], material, glm::vec3(100.0f, 0.0, -60.0), glm::vec3(2.0));
		m_Entities.emplace_back(&m_Models[MHOUSES3], material, glm::vec3(50.0f, 0.0, -60.0), glm::vec3(2.0));
		m_Entities.emplace_back(&m_Models[MHOUSES3], material, glm::vec3(50.0f, 0.0, -100.0), glm::vec3(2.0));
		m_Entities.emplace_back(&m_Models[MTEMPLE1], material, glm::vec3(160.0f, 0.0, -180.0), glm::vec3(1.0));

		m_Entities.emplace_back(&m_Models[MTEMPLE2], material, glm::vec3(50.0f, 0.0, -5.0), glm::vec3(4.3), glm::radians(-90.0f));
		m_Entities.emplace_back(&m_Models[MWOODENBOX], material, glm::vec3(-25.0f, 0.5, -37.7), glm::vec3(0.5));
		m_Entities.emplace_back(&m_Models[MWOODENBOX], material, glm::vec3(-25.0f, 1.0, -37.3), glm::vec3(0.5), glm::radians(30.0f));
		m_Entities.emplace_back(&m_Models[MWOODENBOX], material, glm::vec3(-25.0f, 0.5, -37.1), glm::vec3(0.5), glm::radians(50.0f));

		m_Entities.emplace_back(&m_Models[MTORCH], material, glm::vec3(0.0, 1.0, -315.0), glm::vec3(0.05));
		m_Entities.emplace_back(&m_Models[MTORCH], material, glm::vec3(-90.0f, 1.0, -148.0), glm::vec3(0.05));
		m_Entities.emplace_back(&m_Models[MTORCH], material, glm::vec3(42.0f, 0.5, -12.0), glm::vec3(0.05));
		m_Entities.emplace_back(&m_Models[MTORCH], material, glm::vec3(90.0f, 0.5, -13.0), glm::vec3(0.05));
		m_Entities.emplace_back(&m_Models[MTORCH], material, glm::vec3(50.0f, 0.5, -200.0), glm::vec3(0.05));
		m_Entities.emplace_back(&m_Models[MTORCH], material, glm::vec3(160.0f, 0.5, -180.0), glm::vec3(0.05));

		m_Entities.emplace_back(&m_Models[MMINIPYRAMIDS], material, glm::vec3(-260.0, 0.0, -100.0), glm::vec3(3.0), glm::radians(-90.0f));
	}

	void Render()
	{
		m_Time = glfwGetTime();

		drawSkybox();
		processDayLight();

		m_Shaders[SHADER_LIGHTING].use();
		bindScene();

		m_Map->Draw(m_Shaders[SHADER_LIGHTING]);

		for (auto& entity : m_Entities)
		{
			if (entity.GetModel() == &m_Models[MSHIP])
				moveAlongEllise(entity);
			entity.Advance();

			glm::mat4 model = entity.GetModelMatrix();
			m_Shaders[SHADER_LIGHTING].setMat4("model", model);

			entity.BindMaterial(m_Shaders[SHADER_LIGHTING]);
			entity.Draw(m_Shaders[SHADER_LIGHTING]);
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

	void ToggleFlashlight()
	{
		m_Flashlight = !m_Flashlight;
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
	bool m_Flashlight;

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

	void drawSkybox()
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
		unsigned char* data = stbi_load("resources/maps/map_clouds.png", &width, &height, &nrComponents, 1);

		m_Map = std::make_shared<Map>(data, width, height, width, height, material);
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
		float a = 500.0f, b = 35.0f, h = 0.0f, k = 240.0f;
		glm::vec3 pos = entity.GetPosition();
		float x = h + a * cos(time / 10);
		float z = k + b * sin(time / 10);
		float prevx = h + a * cos((time - 1) / 10);
		float prevz = k + b * sin((time - 1) / 10);
		
		glm::vec3 dir = glm::normalize(glm::vec3(x, pos.y, z) - pos);
		pos = glm::vec3(x, pos.y, z);
		
		float dot = glm::dot(dir, entity.GetDirection());
		float angle = std::acos(dot);
		if (angle >= 0)
		{
			entity.Rotate(entity.GetRotationAngle() - angle);
		}
		/*else
		{
			angle = 0.000005267;
			entity.Rotate(entity.GetRotationAngle() - angle);
		}*/

		//std::cout << "Dot: " << dot << " Angle: " << angle << std::endl;
		//std::cout << "Dir new: " << '(' << dir.x << ", " << dir.y << ", " << dir.z << ')' << std::endl;
		//std::cout << "Dir curr: " << '(' << entity.GetDirection().x << ", " << entity.GetDirection().y << ", " << entity.GetDirection().z << ')' << std::endl;
		//std::cout << " Pos new: " << '(' << x << ", " << 0.0 << ", " << z << ')' << std::endl;
		//std::cout << " Pos prev: " << '(' << prevx << ", " << 0.0 << ", " << prevz << ')' << std::endl;
		//std::cout << "------" << std::endl;

		entity.SetPosition(pos);
		entity.SetDirection(dir);
		//entity.Rotate(glm::radians(anglePhi));
		//float angle = entity.GetRotationAngle();
		//front.x = cos(glm::radians(anglePhi)) * cos(glm::radians(0.0));
		//front.y = sin(glm::radians(0.0));
		//front.z = sin(glm::radians(anglePhi)) * cos(glm::radians(0.0));
	}

	void processDayLight()
	{
		float time = m_InitialTime + m_Time * m_DeltaTime;
		float sinValue = glm::sin(glm::radians(time));
		float cosValue = glm::cos(glm::radians(time));

		//normal distribution N(3.9, 2)
		float daytime = 1 / sqrt(2.0 * PI * 2.0) * powf(E, -0.5 * ((3.5 * abs(sinValue) - 3.9) * (3.5 * abs(sinValue) - 3.9)) / 2.0);
		daytime *= 3.7;

		if (daytime > 1.0)
			daytime = 1.0;

		if (daytime <= 0.4)
		{
			m_Night = true;
		}
		else
		{
			m_Night = false;
		}


		m_SunColor = glm::vec3(daytime);

		m_Shaders[SHADER_SKYBOX].use();

		if(daytime < 0.05)
			m_Shaders[SHADER_SKYBOX].setFloat("time", 0.0);
		else
			m_Shaders[SHADER_SKYBOX].setFloat("time", daytime);

		if (sinValue < 0.0)
			m_LightDir = glm::vec3(-cosValue, sinValue, -0.1);
		else
			m_LightDir = glm::vec3(cosValue, -sinValue, -0.1);

	}

	void createShaders()
	{
		m_Shaders.emplace_back("resources/shaders/vertex.glsl", "resources/shaders/fragment.glsl");
		m_Shaders.emplace_back("resources/shaders/skybox_vertex.glsl", "resources/shaders/skybox_fragment.glsl");

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


		int plCnt = 6;
		m_Shaders[SHADER_LIGHTING].setInt("nPointLights", plCnt);

		for (int i = 0; i < plCnt; i++)
		{
			std::string istr = std::to_string(i);
			m_Shaders[SHADER_LIGHTING].setVec3("pointLight[" + istr + "].color", glm::vec3(1.0));
			m_Shaders[SHADER_LIGHTING].setFloat("pointLight[" + istr + "].constant", 1.0);
			m_Shaders[SHADER_LIGHTING].setFloat("pointLight[" + istr + "].linear", 0.045);
			m_Shaders[SHADER_LIGHTING].setFloat("pointLight[" + istr + "].quadratic", 0.0007);
		}

		m_Shaders[SHADER_LIGHTING].setBool("flashlight", m_Flashlight);

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
		m_Shaders[SHADER_LIGHTING].setVec3("pointLight[3].position", pointLightPositions[3]);
		m_Shaders[SHADER_LIGHTING].setVec3("pointLight[4].position", pointLightPositions[4]);
		m_Shaders[SHADER_LIGHTING].setVec3("pointLight[5].position", pointLightPositions[5]);
		m_Shaders[SHADER_LIGHTING].setBool("flashlight", m_Flashlight);

	}

	void uploadModels()
	{
		m_Models.emplace_back("resources/models/egypt/pyramid1.obj");
		m_Models.emplace_back("resources/models/egypt/pyramid2.obj");
		m_Models.emplace_back("resources/models/egypt/pyramid3.obj");
		m_Models.emplace_back("resources/models/egypt/temple1.obj");
		m_Models.emplace_back("resources/models/egypt/temple2.obj");
		m_Models.emplace_back("resources/models/egypt/temple3.obj");
		m_Models.emplace_back("resources/models/egypt/obelisk.obj");
		m_Models.emplace_back("resources/models/egypt/houses3.obj");
		m_Models.emplace_back("resources/models/egypt/ship.obj");
		

		Model woodenBox;
		Texture tex_diffuse;
		tex_diffuse.id = loadTexture("resources/textures/woodenbox_diffuse.png");
		tex_diffuse.type = "texture_diffuse";
		Texture tex_specular;
		tex_specular.id = loadTexture("resources/textures/woodenbox_specular.png");
		tex_specular.type = "texture_specular";
		woodenBox.CommitGeometry(woodenBoxVerts, woodenboxIndices, { tex_diffuse, tex_specular });
		m_Models.push_back(woodenBox);
		
		m_Models.emplace_back("resources/models/egypt/torch.obj");
		m_Models.emplace_back("resources/models/egypt/mini_pyramids.obj");

	}

	void setPointLights()
	{
		pointLightPositions.push_back(glm::vec3(0.0, 2.0, -315.0));
		pointLightPositions.push_back(glm::vec3(-90.0f, 2.0, -148.0));
		pointLightPositions.push_back(glm::vec3(42.0f, 2.0, -12.0));
		pointLightPositions.push_back(glm::vec3(90.0f, 2.0, -13.0));
		pointLightPositions.push_back(glm::vec3(50.0f, 2.0, -200.0));
		pointLightPositions.push_back(glm::vec3(160.0f, 2.0, -180.0));
	}

};