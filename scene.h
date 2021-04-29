#pragma once

#include "model.h"
#include "camera.h"


class Scene
{
public:
	Scene() = default;
	void Update()
	{
		int x;
	}

	void Init()
	{

	}
private:
	std::vector<Model> m_Models;
	std::vector<Camera> m_Cameras;
	Camera m_MainCamera;
};