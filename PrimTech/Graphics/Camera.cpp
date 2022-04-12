#include "Camera.h"

Camera::Camera():
	m_position(0,0,0), m_rotaion(0,0,0), m_offset(0,0,0)
{}

void Camera::SetPerspective(float fovDeg, float aspectRatio, float nearZ, float farZ)
{
	float fovRad = (fovDeg / 360) * d::XM_2PI;
	m_projM = d::XMMatrixPerspectiveFovLH(fovRad, aspectRatio, nearZ, farZ);
}

void Camera::SetPosition(float x, float y, float z)
{
	m_position = sm::Vector3(x, y, z);
	UpdateView();
}

void Camera::SetPosition(const sm::Vector3 v)
{
	m_position = v;
	UpdateView();
}

void Camera::SetRotation(float x, float y, float z)
{
	m_rotaion = sm::Vector3(x,y,z);
	UpdateView();
}

void Camera::SetRotation(const sm::Vector3 v)
{
	m_rotaion = v;
	UpdateView();
}

void Camera::Move(float x, float y, float z)
{
	m_position += sm::Vector3(x, y, z);
	UpdateView();
}

void Camera::Move(const sm::Vector3 v)
{
	m_position += v;
	UpdateView();
}

void Camera::Rotate(float x, float y, float z)
{
	m_rotaion += sm::Vector3(x, y, z);
	UpdateView();
}

void Camera::Rotate(const sm::Vector3 v)
{
	m_rotaion += v;
	UpdateView();
}

sm::Matrix Camera::GetViewM() const
{
	return m_viewM;
}

sm::Matrix Camera::GetProjM() const
{
	return m_viewM;
}

void Camera::UpdateView()
{
	d::XMMATRIX camRot = d::XMMatrixRotationRollPitchYawFromVector(m_rotaion);

	d::XMVECTOR camTarget = d::XMVector3TransformCoord({ 0,0,1 }, camRot);
	camTarget += m_position;

	d::XMVECTOR upDir = d::XMVector2TransformCoord({ 0,1,0 }, d::XMMatrixRotationRollPitchYawFromVector(m_rotaion));

	m_viewM = d::XMMatrixLookAtLH(m_position, camTarget, upDir);

	//d::XMMATRIX vecRotMatrix = d::XMMatrixRotationRollPitchYawFromVector(m_rotaion);

	//m_forwardV = d::XMVector3TransformCoord({ 0,0,1 }, vecRotMatrix);
	//m_leftV = d::XMVector3TransformCoord({ -1,0,0 }, vecRotMatrix);
}
