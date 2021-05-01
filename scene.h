#pragma once

#include <omp.h>

#include "model.h"
#include "camera.h"
#include "material.h"
#include "entity.h"


static unsigned int loadCubemapTexture(std::vector<std::string> faces)
{
	unsigned int texId;
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texId);

	int width, height, nrChannels;
	for (int i = 0; i < faces.size(); i++)
	{
		unsigned char * data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return texId;
}


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

		m_SkyboxShader = std::make_shared<Shader>("skybox_vertex.glsl", "skybox_fragment.glsl");
		m_SkyboxShader->use();
		m_SkyboxShader->setInt("skyboxDay", 0);
		m_SkyboxShader->setInt("skyboxNight", 1);
		createSkybox();


		//m_Models.emplace_back("models/egypt/pyramids.obj");
		m_Models.emplace_back("models/egypt/temple1.obj");
		//m_Models.emplace_back("models/egypt/temple2.obj");
		//m_Models.emplace_back("models/egypt/Obelisk+mini pyramids.obj");

		Material material(glm::vec3(0.2), glm::vec3(0.5), glm::vec3(1.0), 32.0, glm::vec3(0.0));

		//m_Entities.emplace_back(&m_Models[i++], material, "lighting", glm::vec3(0.0f), glm::vec3(10.0));
		m_Entities.emplace_back(&m_Models[i++], material, "lighting", glm::vec3(50.0f, 0.0, 0.0), glm::vec3(1.0));
		//m_Entities.emplace_back(&m_Models[i++], material, "lighting", glm::vec3(-50.0f, 0.0, 0.0), glm::vec3(1.0));
		//m_Entities.emplace_back(&m_Models[i++], material, "lighting", glm::vec3(0.0f, 0.0, 50.0), glm::vec3(1.0));
	}

	void Render()
	{
		float time = glfwGetTime();
		time /= 10;
		float radius = 20;
		m_SunPosition = glm::vec3(radius * cos(time), radius * sin(time), m_SunPosition.z);

		drawSkybox(time);

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
	std::shared_ptr<Shader> m_SkyboxShader;
	std::vector<std::shared_ptr<Shader>> m_Shaders;
	unsigned int skyVAO;
	unsigned int cubeMapTextureDay;
	unsigned int cubeMapTextureNight;

private:
	void createSkybox()
	{
		float skyboxVerts[] = {
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};


		std::vector<std::string> facesDay
		{
			"skybox/skybox2/right.jpg",
			"skybox/skybox2/left.jpg",
			"skybox/skybox2/top.jpg",
			"skybox/skybox2/bottom.jpg",
			"skybox/skybox2/front.jpg",
			"skybox/skybox2/back.jpg",
		};

		std::vector<std::string> facesNight
		{
			"skybox/skybox/right.jpg",
			"skybox/skybox/left.jpg",
			"skybox/skybox/top.jpg",
			"skybox/skybox/bottom.jpg",
			"skybox/skybox/front.jpg",
			"skybox/skybox/back.jpg",
		};

		glGenVertexArrays(1, &skyVAO);
		glBindVertexArray(skyVAO);

		unsigned int skyVBO;
		glGenBuffers(1, &skyVBO);
		glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVerts), skyboxVerts, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		cubeMapTextureDay = loadCubemapTexture(facesDay);
		cubeMapTextureNight = loadCubemapTexture(facesNight);
	}

	void drawSkybox(float time) const
	{
		glDepthMask(GL_FALSE);
		m_SkyboxShader->use();
		glm::mat4 proj = m_MainCamera->GetProjectionMatrix();
		glm::mat4 view = glm::mat4(glm::mat3(m_MainCamera->GetViewMatrix()));
		m_SkyboxShader->setMat4("projection", proj);
		m_SkyboxShader->setMat4("view", view);
		m_SkyboxShader->setFloat("time", abs(sin(time / 2)));


		glBindVertexArray(skyVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureDay);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureNight);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);
	}
};