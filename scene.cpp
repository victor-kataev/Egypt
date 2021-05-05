#include "scene.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

Scene::Scene()
{
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

void Scene::Init()
{

	setUpShaders();

	createSkybox();

	Material material(glm::vec3(0.2), glm::vec3(0.5), glm::vec3(1.0), 32.0);
	createMap(material);

	uploadModels();
	setPointLights();


	m_Entities.emplace_back(&m_Models[MPYRAMID1], material, glm::vec3(0.0f, 0.0f, -310.0f), glm::vec3(8.0));
	m_Entities.emplace_back(&m_Models[MPYRAMID2], material, glm::vec3(300.0f, 0.0, -280.0), glm::vec3(8.0));
	m_Entities.emplace_back(&m_Models[MPYRAMID3], material, glm::vec3(-300.0f, 0.0, -180.0), glm::vec3(8.0));
	m_Entities.emplace_back(&m_Models[MSHIP], material, glm::vec3(0.0f, -3.5, 130.0), glm::vec3(0.2), glm::radians(90.0));
	m_Entities[m_Entities.size() - 1].SetDirection(glm::vec3(1.0, 0.0, 0.0));
	m_Entities[m_Entities.size() - 1].SetVelocity(0.5);
	m_Entities[m_Entities.size() - 1].AttachCamera(&m_Cameras[1], glm::vec3(2.0, 5.0, -1.0));
	m_Entities.emplace_back(&m_Models[MWOODENBOX], material, glm::vec3(0.0f, 10.5, 0.0), glm::vec3(1.0));

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

void Scene::Render()
{
	m_Time = glfwGetTime();

	drawSkybox(m_Time);
	processDayLight();

	m_Shaders[SHADER_LIGHTING].use();
	bindScene();

	drawMap();

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
