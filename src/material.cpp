#include "material.h"

namespace kataevic
{

	Material::Material(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess)
	{
		AmbientColor = ambient;
		DiffuseColor = diffuse;
		SpecularColor = specular;
		Shininess = shininess;
	}

	void Material::Bind(Shader& shader)
	{
		shader.setFloat("material.shininess", Shininess);
		shader.setVec3("material.ambient", AmbientColor);
		shader.setVec3("material.diffuse", DiffuseColor);
		shader.setVec3("material.specular", SpecularColor);
	}

}