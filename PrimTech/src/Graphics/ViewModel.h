#pragma once
#include"RenderShapes.h"

class ViewModel
{
public:
	void Init(std::string path, CameraHandler* cam);
	void Draw();
	Model m_model;
private:
	CameraHandler* mp_cam = nullptr;
};