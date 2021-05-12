#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "camera.h"
#include "material.h"
#include "entity.h"
#include "map.h"
#include "utilities.h"


#define PI 3.14159265359
#define E  2.71828182845

#define MPYRAMID1		0
#define MPYRAMID2		1
#define MPYRAMID3		2
#define MTEMPLE1		3
#define MTEMPLE2		4
#define MTEMPLE3		5
#define MOBELISK		6
#define MHOUSES3		7
#define MSHIP			8
#define MWOODENBOX		9
#define MTORCH			10
#define MMINIPYRAMIDS	11


#define SHADER_LIGHTING 0
#define SHADER_SKYBOX   1


class Scene
{
public:
	Scene();
	void Init();
	void Render();
	void SetMainCamera(int id);
	Camera* GetCamera();
	void ToggleFlashlight();

private:
	void createSkybox();
	void drawSkybox();
	void createMap(Material material);
	void handleCollision(Entity& entity);
	void moveAlongEllise(Entity& entity);
	void processDayLight();
	void createShaders();
	void bindScene();
	void uploadModels();
	void setPointLights();

private:
	std::vector<Model> m_Models;
	std::vector<Entity> m_Entities;
	std::vector<Camera> m_Cameras;
	Camera * m_MainCamera;
	glm::vec3 m_SunPosition;
	glm::vec3 m_SunColor;
	glm::vec3 m_LightDir;
	std::vector<Shader> m_Shaders;
	Model m_Skybox;
	std::shared_ptr<Map> m_Map;
	bool m_Night;
	float m_Time;
	float m_InitialTime;
	float m_DeltaTime;
	bool m_Flashlight;
	unsigned int cubeMapTextureDay;
	unsigned int cubeMapTextureNight;
	std::vector<glm::vec3> pointLightPositions;
};