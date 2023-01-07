#include "ViewModel.h"

void ViewModel::Init(std::string path, CameraHandler* cam)
{
	mp_cam = cam;
	m_model.Init(path);
}

void ViewModel::Draw()
{
	m_model.SetPosition(mp_cam->GetCurrentCamera()->GetPosition());
	m_model.SetRotation(-mp_cam->GetCurrentCamera()->GetRotation().x, mp_cam->GetCurrentCamera()->GetRotation().y + d::XM_PI, -mp_cam->GetCurrentCamera()->GetRotation().z);
	m_model.Draw();
}
