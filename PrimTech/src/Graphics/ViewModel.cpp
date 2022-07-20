#include "ViewModel.h"

void ViewModel::Init(std::string path, Camera* cam)
{
	mp_cam = cam;
	m_model.Init(path);
}

void ViewModel::Draw()
{
	m_model.SetPosition(mp_cam->GetPosition());
	m_model.SetRotation(-mp_cam->GetRotation().x, mp_cam->GetRotation().y + d::XM_PI, -mp_cam->GetRotation().z);
	m_model.Draw();
}
