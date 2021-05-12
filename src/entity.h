#pragma once

#include "camera.h"
#include "model.h"
#include "material.h"

namespace kataevic
{

	class Entity
	{
	public:
		Entity(Model* model, Material material, const glm::vec3& pos, const glm::vec3& scale, float angle = 0.0, glm::vec3 axis = glm::vec3(0.0, 1.0, 0.0));

		glm::vec3 GetPosition() const;
		glm::mat4 GetModelMatrix() const;
		void Draw(const Shader& shader) const;
		Material& GetMaterial();
		void Rotate(float angle, glm::vec3 axis = glm::vec3(0.0, 1.0, 0.0));
		void Advance();
		void AttachCamera(Camera* cam, glm::vec3 ancorPoint = glm::vec3(0.0));
		void SetPosition(glm::vec3 pos);
		void SetDirection(const glm::vec3 dir);
		void SetVelocity(float vel);
		glm::vec3 GetDirection() const;
		Model* GetModel() const;
		float GetRotationAngle() const;
		void BindMaterial(Shader& shader);

	private:
		void applyTransformations();

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

	};
}