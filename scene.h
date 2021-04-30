#pragma once

#include "model.h"
#include "camera.h"
#include "material.h"
#include "entity.h"

class Scene
{
public:
	Scene()
	{
		//Cameras
		m_Cameras.emplace_back(glm::vec3(0.0, 0.0, 3.0), -90.0, 0.0);
		m_Cameras.emplace_back(glm::vec3(-10.0, 0.0, 3.0), -90.0, 0.0);
		m_Cameras.emplace_back(glm::vec3(10.0, 20.0, 3.0), -90.0, 0.0);
		m_MainCamera = &m_Cameras[0];

		m_SunPosition = glm::vec3(10.0f);
		m_SunColor = glm::vec3(1.0f);
	}

	void Init()
	{
		int i = 0;

		

		std::shared_ptr<Shader> shader = std::make_shared<Shader>("vertex.glsl", "fragment.glsl");
		m_Shader = shader;
		shader->use();
		shader->setInt("material.texture_diffuse1", 0);
		shader->setInt("material.texture_specular1", 1);
		//shader->setVec3("lightPos", m_SunPosition);
		//shader->setVec3("viewerPos", m_MainCamera.Position);

		Material material(shader, glm::vec3(0.2f), glm::vec3(0.5f), m_SunColor, 32.0f);

		m_Models.emplace_back("models/egypt/pyramids.obj");

		m_Entities.emplace_back(&m_Models[i], material, glm::vec3(0.0f));
	}

	void Update()
	{
		for (auto& entity : m_Entities)
		{
			entity.Update(m_MainCamera, m_SunPosition, m_SunColor);
			entity.Draw();
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
	std::shared_ptr<Shader> m_Shader;
};