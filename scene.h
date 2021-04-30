#pragma once

#include "model.h"
#include "camera.h"
#include "material.h"
#include "entity.h"

class Scene
{
public:
	Scene()
		: m_MainCamera(glm::vec3(0.0, 0.0, 3.0))
	{
		m_SunPosition = glm::vec3(10.0f);
		m_SunColor = glm::vec3(1.0f);
	}

	void Init()
	{
		int i = 0;

		//Cameras

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

#if 0

		m_Shader->use();
		glm::mat4 projection = m_MainCamera.GetProjectionMatrix();
		glm::mat4 view = m_MainCamera.GetViewMatrix();
		m_Shader->setMat4("projection", projection);
		m_Shader->setMat4("view", view);
		m_Shader->setVec3("lightPos", m_SunPosition);
		m_Shader->setVec3("viewerPos", m_MainCamera.Position);
		m_Shader->setVec3("light.ambient", m_SunColor * glm::vec3(0.2f));
		m_Shader->setVec3("light.diffuse", m_SunColor * glm::vec3(0.5f));
		m_Shader->setVec3("light.specular", m_SunColor);
		glm::mat4 model = glm::mat4(1.0);
		m_Shader->setMat4("model", model);
#endif // 0

	}

	Camera& GetCamera()
	{
		return m_MainCamera;
	}
private:
	std::vector<Model> m_Models;
	std::vector<Entity> m_Entities;
	std::vector<Camera> m_Cameras;
	Camera m_MainCamera;
	glm::vec3 m_SunPosition;
	glm::vec3 m_SunColor;
	std::shared_ptr<Shader> m_Shader;
};