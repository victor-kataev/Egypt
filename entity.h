#pragma once

#include "model.h"
#include "material.h"

class Entity
{
public:
	Entity(Model* model, Material material, const glm::vec3& pos, const glm::vec3 & scale, float angle = 0.0, glm::vec3 axis = glm::vec3(0.0, 1.0, 0.0))
		: 
		m_Model(model),
		m_Material(material),
		m_WorldPos(pos),
		m_Scale(scale),
		m_RotAngle(angle),
		m_RotAxis(axis),
		m_Direction(0.0, 1.0, 0.0),
		m_Velocity(0.0),
		m_Camera(NULL)
	{
		ApplyTransformations();
	}

	glm::vec3 GetPosition() const
	{
		return m_WorldPos;
	}

	glm::mat4 GetModelMatrix() const
	{
		return m_ModelMatrix;
	}

	void Draw(const Shader& shader) const
	{
			m_Model->DrawElements(shader);
	}

	Material& GetMaterial()
	{
		return m_Material;
	}

	void Rotate(float angle, glm::vec3 axis)
	{
		m_ModelMatrix = glm::mat4(1.0);
		m_ModelMatrix = glm::translate(m_ModelMatrix, m_WorldPos);
		m_ModelMatrix = glm::rotate(m_ModelMatrix, angle, axis);
		m_ModelMatrix = glm::scale(m_ModelMatrix, m_Scale);
	}

	void Advance()
	{
		m_WorldPos += m_Velocity * m_Direction;
		//m_ModelMatrix = glm::translate(m_ModelMatrix, m_WorldPos);
		ApplyTransformations();

		if (m_Camera)
			m_Camera->Position = m_WorldPos + m_CameraAncorPoint;
	}

	void AttachCamera(Camera* cam, glm::vec3 ancorPoint = glm::vec3(0.0))
	{
		m_Camera = cam;
		m_CameraAncorPoint = ancorPoint;
	}

	void SetPosition(glm::vec3 pos)
	{
		m_WorldPos = pos;
	}

	void SetDirection(const glm::vec3 dir)
	{
		m_Direction = glm::normalize(dir);
	}

	void SetVelocity(float vel)
	{
		m_Velocity = vel;
	}

	glm::vec3 GetDirection() const
	{
		return m_Direction;
	}

	Model* GetModel() const
	{
		return m_Model;
	}

	float GetRotationAngle() const
	{
		return m_RotAngle;
	}

private:
	Model* m_Model;
	Material m_Material;
	glm::vec3 m_WorldPos;
	glm::vec3 m_Scale;
	glm::vec3 m_Direction;
	float m_Velocity;
	float m_RotAngle;
	glm::vec3 m_RotAxis;
	glm::mat4 m_ModelMatrix;
	Camera* m_Camera;
	glm::vec3 m_CameraAncorPoint;

private:
	void ApplyTransformations()
	{
		m_ModelMatrix = glm::mat4(1.0);
		m_ModelMatrix = glm::translate(m_ModelMatrix, m_WorldPos);
		m_ModelMatrix = glm::rotate(m_ModelMatrix, m_RotAngle, m_RotAxis);
		m_ModelMatrix = glm::scale(m_ModelMatrix, m_Scale);
	}
};