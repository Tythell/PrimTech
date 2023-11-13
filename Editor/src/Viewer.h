#pragma once
#include "PrimTech.h"

class Viewer
{
public:
	Viewer();
	~Viewer();

	bool Run();
private:
	void ControlCam();

	pt::PrimTech m_engine;

	PrimtTech::ImGuiHandler m_pGui;

	pt::Entity* m_avatar = nullptr;
	pt::MeshRef* m_mesh = nullptr;

	//pt::Entity* m_camEnt = nullptr;
	pt::Camera* m_cam = nullptr;
};