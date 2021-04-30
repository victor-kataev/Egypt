#pragma once

#include <omp.h>

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

		m_LightingShader = std::make_shared<Shader>("vertex.glsl", "fragment.glsl");
		m_LightingShader->use();
		m_LightingShader->setInt("material.texture_diffuse1", 0);
		m_LightingShader->setInt("material.texture_specular1", 1);

		m_Models.emplace_back("models/egypt/pyramids.obj");
		m_Models.emplace_back("models/egypt/temple1.obj");
		m_Models.emplace_back("models/egypt/temple2.obj");
		m_Models.emplace_back("models/egypt/Obelisk+mini pyramids.obj");

		Material material(glm::vec3(0.2), glm::vec3(0.5), glm::vec3(1.0), 32.0, glm::vec3(0.0));

		m_Entities.emplace_back(&m_Models[i++], material, "lighting", glm::vec3(0.0f), glm::vec3(10.0));
		m_Entities.emplace_back(&m_Models[i++], material, "lighting", glm::vec3(50.0f, 0.0, 0.0), glm::vec3(1.0));
		m_Entities.emplace_back(&m_Models[i++], material, "lighting", glm::vec3(-50.0f, 0.0, 0.0), glm::vec3(1.0));
		m_Entities.emplace_back(&m_Models[i++], material, "lighting", glm::vec3(0.0f, 0.0, 50.0), glm::vec3(1.0));
	}

	void Render()
	{
		float time = glfwGetTime();
		time /= 10;
		float radius = 20;
		m_SunPosition = glm::vec3(radius * cos(time), radius * sin(time), m_SunPosition.z);

		for (auto& entity : m_Entities)
		{
			if (entity.GetShaderType() == "lighting")
			{
				glm::mat4 proj = m_MainCamera->GetProjectionMatrix();
				glm::mat4 view = m_MainCamera->GetViewMatrix();
				glm::mat4 model = entity.GetModelMatrix();

				m_LightingShader->use();
				m_LightingShader->setMat4("projection", proj);
				m_LightingShader->setMat4("view", view);
				m_LightingShader->setMat4("model", model);
				m_LightingShader->setVec3("lightPos", m_SunPosition);
				m_LightingShader->setVec3("viewerPos", m_MainCamera->Position);
				m_LightingShader->setFloat("material.shininess", entity.GetMaterial().Shininess);
				m_LightingShader->setVec3("light.ambient", entity.GetMaterial().AmbientColor * m_SunColor);
				m_LightingShader->setVec3("light.diffuse", entity.GetMaterial().DiffuseColor * m_SunColor);
				m_LightingShader->setVec3("light.specular", entity.GetMaterial().SpecularColor * m_SunColor);
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
	std::shared_ptr<Shader> m_LightingShader;
};