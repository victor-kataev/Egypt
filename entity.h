#pragma once

#include "model.h"
#include "material.h"

class Entity
{
public:
	Entity(int model, const Material& material, const glm::vec3& pos)
	{
		m_Model = model;
		m_Material = material;
		m_WorldPos = pos;
	}

	void Update(const Camera& camera, const glm::vec3& sunPos, const glm::vec3& sunColor)
	{

	}

private:
	int m_Model;
	Material m_Material;
	glm::vec3 m_WorldPos;
};