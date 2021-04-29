#pragma once

#include "model.h"
#include "camera.h"
#include "material.h"
#include "entity.h"

class Scene
{
public:
	Scene(int screenWidth, int screenHeight)
	{
		m_ProjectionMatrix = glm::perspective(glm::radians(45.0f), (float)screenWidth / screenHeight, 0.1f, 1000.f);
		m_ViewMatrix = m_MainCamera.GetViewMatrix();
		m_SunPosition = glm::vec3(20.0f);
		m_SunColor = glm::vec3(1.0f);
	}

	void Init()
	{
		int i = 0;

		//Cameras

		std::shared_ptr<Shader> shader = std::make_shared<Shader>("vertex.glsl", "fragment.glsl");
		shader->use();
		shader->setInt("material.texture_diffuse1", 0);
		shader->setInt("material.texture_specular1", 1);
		shader->setMat4("projection", m_ProjectionMatrix);
		shader->setMat4("view", m_ViewMatrix);
		shader->setVec3("lightPos", m_SunPosition);
		shader->setVec3("viewerPos", m_MainCamera.Position);

		Material material(shader, m_SunColor * glm::vec3(0.2f), m_SunColor * glm::vec3(0.5f), m_SunColor);

		m_Models.emplace_back("models/egypt/pyramids.obj");

		m_Entities.emplace_back(i, material, glm::vec3(10.0f));
	}

	void Update()
	{
		for (auto & entity : m_Entities)
		{
			entity.Update(m_MainCamera, m_SunPosition, m_SunColor);
		}
	}
private:
	std::vector<Model> m_Models;
	std::vector<Entity> m_Entities;
	std::vector<Camera> m_Cameras;
	Camera m_MainCamera;
	glm::vec3 m_SunPosition;
	glm::vec3 m_SunColor;
	glm::mat4 m_ProjectionMatrix;
	glm::mat4 m_ViewMatrix;
};