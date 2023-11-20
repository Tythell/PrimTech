#pragma once
#include "PrimTech.h"

class Viewer
{
public:
	Viewer();
	~Viewer();

	bool Run();
private:
	bool m_enables[12];
	void ControlCam();
	void InitImguiWindows();
	void UpdateToggles();
	void UpdateTexture();

	pt::PrimTech m_engine;

	PrimtTech::ImGuiHandler m_pGui;

	pt::Entity* m_avatar = nullptr;
	pt::MeshRef* m_mesh = nullptr;

	pt::Camera* m_cam = nullptr;

	pt::Entity* m_pCamEnt = nullptr;;
};