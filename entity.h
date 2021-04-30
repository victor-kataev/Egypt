#pragma once

#include "model.h"
#include "material.h"

class Entity
{
public:
	Entity(Model* model, const Material& material, const glm::vec3& pos)
	{
		m_Model = model;
		m_Material = material;
		m_WorldPos = pos;
	}

	void Update(Camera& camera, const glm::vec3& sunPos, const glm::vec3& sunColor)
	{
		std::shared_ptr<Shader> shader = m_Material.Shader;
		shader->use();
		glm::mat4 projection = camera.GetProjectionMatrix();
		glm::mat4 view = camera.GetViewMatrix();
		shader->setMat4("projection", projection);
		shader->setMat4("view", view);
		shader->setVec3("lightPos", sunPos);
		shader->setVec3("viewerPos", camera.Position);
		shader->setVec3("light.ambient", sunColor * m_Material.AmbientColor);
		shader->setVec3("light.diffuse", sunColor * m_Material.DiffuseColor);
		shader->setVec3("light.specular", sunColor * m_Material.SpecularColor);
		shader->setFloat("material.shininess", m_Material.Shininess);

	}

	void Draw() const
	{
		std::shared_ptr<Shader> shader = m_Material.Shader;
		shader->use();
		glm::mat4 model = glm::mat4(1.0);
		model = glm::translate(model, m_WorldPos);
		shader->setMat4("model", model);
		m_Model->Draw(*m_Material.Shader);
	}
	

private:
	Model* m_Model;
	Material m_Material;
	glm::vec3 m_WorldPos;
};