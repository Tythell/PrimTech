#pragma once
#include"RenderShapes.h"

class ViewModel
{
public:
	void Init(std::string path, Camera* cam);
	void Draw();
	Model m_model;
private:
	Camera* mp_cam = nullptr;
};