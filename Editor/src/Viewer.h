#pragma once
#include "PrimTech.h"
#include "Comlib/Comlib.h"
#include "WindowFuncs.h"

class Viewer
{
public:
	Viewer();
	~Viewer();

	bool Run();
private:
	char m_reloadKey = 'S';
	uint m_sleepTimeBeforeReload = 100;
	ToggleWindowStructure m_windowStruct;
	Gui_MenuToggles m_guiToggles;

	std::queue<std::string> m_commands;

	void ControlCam();
	void InitImguiWindows();
	void UpdateToggles();
	void UpdateTexture();
	bool ComlibUpdate(PrimtTech::TextureMap*& pTexture);
	void UpdateCommands();

	bool m_initSharedMem;
	comlib::RingBuffer* m_comlib = nullptr;
	uint m_msgSize;
	uint m_bufferSize;

	pt::PrimTech m_engine;

	PrimtTech::ImGuiHandler m_pGui;

	pt::Entity* m_avatar = nullptr;
	pt::MeshRef* m_mesh = nullptr;

	pt::Camera* m_cam = nullptr;

	pt::Entity* m_pCamEnt = nullptr;
	std::string currentSkinFile = "";
};