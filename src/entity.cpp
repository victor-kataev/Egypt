#include "entity.h"

namespace kataevic
{

	Entity::Entity(Model* model, Material material, const glm::vec3& pos, const glm::vec3& scale, float angle, glm::vec3 axis)
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
		applyTransformations();
	}

	glm::vec3 Entity::GetPosition() const
	{
		return m_WorldPos;
	}

	glm::mat4 Entity::GetModelMatrix() const
	{
		return m_ModelMatrix;
	}

	void Entity::Draw(const Shader& shader) const
	{
		m_Model->DrawElements(shader);
	}

	Material& Entity::GetMaterial()
	{
		return m_Material;
	}

	void Entity::Rotate(float angle, glm::vec3 axis)
	{
		m_RotAngle = angle;
		m_RotAxis = axis;
	}

	void Entity::Advance()
	{
		applyTransformations();

		if (m_Camera)
			m_Camera->Position = m_WorldPos + m_CameraAncorPoint;
	}

	void Entity::AttachCamera(Camera* cam, glm::vec3 ancorPoint)
	{
		m_Camera = cam;
		m_Camera->Movable = false;
		m_CameraAncorPoint = ancorPoint;
	}

	void Entity::SetPosition(glm::vec3 pos)
	{
		m_WorldPos = pos;
	}

	void Entity::SetDirection(const glm::vec3 dir)
	{
		m_Direction = glm::normalize(dir);
	}

	void Entity::SetVelocity(float vel)
	{
		m_Velocity = vel;
	}

	glm::vec3 Entity::GetDirection() const
	{
		return m_Direction;
	}

	Model* Entity::GetModel() const
	{
		return m_Model;
	}

	float Entity::GetRotationAngle() const
	{
		return m_RotAngle;
	}

	void Entity::BindMaterial(Shader& shader)
	{
		m_Material.Bind(shader);
	}

	void Entity::applyTransformations()
	{
		m_ModelMatrix = glm::mat4(1.0);
		m_ModelMatrix = glm::translate(m_ModelMatrix, m_WorldPos);
		m_ModelMatrix = glm::rotate(m_ModelMatrix, m_RotAngle, m_RotAxis);
		m_ModelMatrix = glm::scale(m_ModelMatrix, m_Scale);
	}
}