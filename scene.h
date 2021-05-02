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

		pointLightPositions.push_back(glm::vec3(-150.0, 50.0, -100.0));
		pointLightPositions.push_back(glm::vec3(50.0, 2.0, -10.0));
		pointLightPositions.push_back(glm::vec3(-70.0, 10.0, -400.0));

		m_LightingShader = std::make_shared<Shader>("vertex.glsl", "fragment.glsl");
		m_LightingShader->use();
		m_LightingShader->setInt("material.texture_diffuse1", 0);
		m_LightingShader->setInt("material.texture_specular1", 1);
		
		m_LightingShader->setFloat("spotlight.constant", 1.0);
		m_LightingShader->setFloat("spotlight.linear", 0.007);
		m_LightingShader->setFloat("spotlight.quadratic", 0.0002);
		
		m_LightingShader->setInt("nPointLights", 3);
		m_LightingShader->setFloat("pointLight[0].constant", 1.0);
		m_LightingShader->setFloat("pointLight[0].linear", 0.007);
		m_LightingShader->setFloat("pointLight[0].quadratic", 0.0002);

		m_LightingShader->setFloat("pointLight[1].constant", 1.0);
		m_LightingShader->setFloat("pointLight[1].linear", 0.007);
		m_LightingShader->setFloat("pointLight[1].quadratic", 0.0002);

		m_LightingShader->setFloat("pointLight[2].constant", 1.0);
		m_LightingShader->setFloat("pointLight[2].linear", 0.007);
		m_LightingShader->setFloat("pointLight[2].quadratic", 0.0002);


		m_SkyboxShader = std::make_shared<Shader>("skybox_vertex.glsl", "skybox_fragment.glsl");
		m_SkyboxShader->use();
		m_SkyboxShader->setInt("skyboxDay", 0);
		m_SkyboxShader->setInt("skyboxNight", 1);
		createSkybox();


		m_Models.emplace_back("models/egypt/pyramids.obj");
		//m_Models.emplace_back("models/egypt/temple1.obj");
		//m_Models.emplace_back("models/egypt/temple2.obj");
		//m_Models.emplace_back("models/egypt/Obelisk+mini pyramids.obj");

		Material material(glm::vec3(0.2), glm::vec3(0.5), glm::vec3(1.0), 32.0);

		m_Entities.emplace_back(&m_Models[i++], material, "lighting", glm::vec3(0.0f), glm::vec3(10.0));
		//m_Entities.emplace_back(&m_Models[i++], material, "lighting", glm::vec3(50.0f, 0.0, 0.0), glm::vec3(1.0));
		//m_Entities.emplace_back(&m_Models[i++], material, "lighting", glm::vec3(-50.0f, 0.0, 0.0), glm::vec3(1.0));
		//m_Entities.emplace_back(&m_Models[i++], material, "lighting", glm::vec3(0.0f, 0.0, 50.0), glm::vec3(1.0));
	}

	void Render()
	{
		float time = glfwGetTime();
		time /= 2;
		float radius = 20;
		//m_SunPosition = glm::vec3(radius * cos(time), radius * sin(time), m_SunPosition.z);
		glm::vec3 lightDir(-0.5f, -0.9, 0.8);

		drawSkybox(time);

		glm::mat4 proj = m_MainCamera->GetProjectionMatrix();
		glm::mat4 view = m_MainCamera->GetViewMatrix();

		m_LightingShader->use();
		m_LightingShader->setMat4("projection", proj);
		m_LightingShader->setMat4("view", view);
		m_LightingShader->setVec3("SunColor", m_SunColor);
		m_LightingShader->setVec3("SunPositon", m_SunPosition);


		for (auto& entity : m_Entities)
		{
			if (entity.GetShaderType() == "lighting")
			{
				
				glm::mat4 model = entity.GetModelMatrix();

				
				m_LightingShader->setMat4("model", model);
				m_LightingShader->setVec3("viewerPos", m_MainCamera->Position);
				m_LightingShader->setFloat("material.shininess", entity.GetMaterial().Shininess);
				m_LightingShader->setVec3("dirLight.dir", lightDir);
				m_LightingShader->setVec3("dirLight.ambient", entity.GetMaterial().AmbientColor);
				m_LightingShader->setVec3("dirLight.diffuse", entity.GetMaterial().DiffuseColor * m_SunColor);
				m_LightingShader->setVec3("dirLight.specular", entity.GetMaterial().SpecularColor * m_SunColor);

				m_LightingShader->setVec3("spotlight.position", m_MainCamera->Position);
				m_LightingShader->setVec3("spotlight.dir", m_MainCamera->Front);
				m_LightingShader->setVec3("spotlight.ambient", entity.GetMaterial().AmbientColor * m_SunColor);
				m_LightingShader->setVec3("spotlight.diffuse", entity.GetMaterial().DiffuseColor * m_SunColor);
				m_LightingShader->setVec3("spotlight.specular", entity.GetMaterial().SpecularColor * m_SunColor);
				m_LightingShader->setFloat("spotlight.cutOff", glm::cos(glm::radians(12.5f)));
				m_LightingShader->setFloat("spotlight.outerCutOff", glm::cos(glm::radians(17.5f)));

				m_LightingShader->setVec3("pointLight[0].position", pointLightPositions[0]);
				m_LightingShader->setVec3("pointLight[0].ambient", entity.GetMaterial().AmbientColor * m_SunColor);
				m_LightingShader->setVec3("pointLight[0].diffuse", entity.GetMaterial().DiffuseColor * m_SunColor);
				m_LightingShader->setVec3("pointLight[0].specular", entity.GetMaterial().SpecularColor * m_SunColor);
				
				m_LightingShader->setVec3("pointLight[1].position", pointLightPositions[1]);
				m_LightingShader->setVec3("pointLight[1].ambient", entity.GetMaterial().AmbientColor * m_SunColor);
				m_LightingShader->setVec3("pointLight[1].diffuse", entity.GetMaterial().DiffuseColor * m_SunColor);
				m_LightingShader->setVec3("pointLight[1].specular", entity.GetMaterial().SpecularColor * m_SunColor);

				m_LightingShader->setVec3("pointLight[2].position", pointLightPositions[2]);
				m_LightingShader->setVec3("pointLight[2].ambient", entity.GetMaterial().AmbientColor * m_SunColor);
				m_LightingShader->setVec3("pointLight[2].diffuse", entity.GetMaterial().DiffuseColor * m_SunColor);
				m_LightingShader->setVec3("pointLight[2].specular", entity.GetMaterial().SpecularColor * m_SunColor);

				//m_LightingShader->setVec3("light.position", m_MainCamera->Position);
				//m_LightingShader->setVec3("light.dir", m_MainCamera->Front);
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
	Model m_Skybox;
	
	//tmp
	unsigned int cubeMapTextureDay;
	unsigned int cubeMapTextureNight;
	std::vector<glm::vec3> pointLightPositions;

private:
	void createSkybox()
	{
		std::vector<float> skyboxVerts = {
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

		m_Skybox.CommitGeometry(skyboxVerts);

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


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureDay);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureNight);
		m_Skybox.DrawArrays();
		glDepthMask(GL_TRUE);
	}
};