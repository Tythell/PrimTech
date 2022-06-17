#include "Camera.h"

Camera::Camera():
	m_position(0,0,0), m_rotaion(0,0,0), m_offset(0,0,0)
{
	UpdateView();
}

void Camera::SetPerspective(float fovDeg, float aspectRatio, float nearZ, float farZ)
{
	float fovRad = (fovDeg / 360) * d::XM_2PI;
	m_projM = d::XMMatrixPerspectiveFovLH(fovRad, aspectRatio, nearZ, farZ);
}

void Camera::SetOrtographic(float width, float height, float nearZ, float farZ)
{
	m_projM = d::XMMatrixOrthographicLH(width, height, nearZ, farZ);
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

void Camera::SetOffset(float x, float y, float z)
{
	m_offset = sm::Vector3(x, y, z);
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

void Camera::Offset(float x, float y, float z)
{
	m_offset += sm::Vector3(x, y, z);
	UpdateView();
}

void Camera::Offset(const sm::Vector3 v)
{
	m_offset += v;
	UpdateView();
}

sm::Matrix Camera::GetViewM() const
{
	return m_viewM;
}

sm::Matrix Camera::GetProjM() const
{
	return m_projM;
}

sm::Vector3 Camera::GetForwardVector() const
{
	return m_forwardV;
}

sm::Vector3 Camera::GetLeftVector() const
{
	return m_leftV;
}

sm::Vector3 Camera::GetUpVector() const
{
	return m_upV;
}

sm::Vector3 Camera::GetOffset() const
{
	return m_offset;
}

sm::Vector3 Camera::GetPosition() const
{
	return m_position;
}

void Camera::UpdateView()
{
	d::XMMATRIX camRot = d::XMMatrixRotationRollPitchYawFromVector(m_rotaion);

	d::XMVECTOR camTarget = d::XMVector3TransformCoord({ 0.f,0.f,1.f }, camRot);
	camTarget += m_position;

	d::XMVECTOR upDir = d::XMVector2TransformCoord({ 0.f,1.f,0.f }, d::XMMatrixRotationRollPitchYawFromVector(m_rotaion));

	if (m_offset.z < 0.f) m_offset.z = 0.f;
	else if (m_offset.z > OFFSETMAX) m_offset.z = OFFSETMAX;
	sm::Matrix offsetMatrix = d::XMMatrixTranslationFromVector(m_offset);
	m_viewM = d::XMMatrixLookAtLH(m_position, camTarget, upDir);


	m_viewM =  m_viewM * offsetMatrix;


	m_forwardV = d::XMVector3TransformCoord({ 0,0,1 }, camRot);
	m_leftV = d::XMVector3TransformCoord({ -1,0,0 }, camRot);
	m_upV = d::XMVector3TransformCoord({ 0.f,1.f,0.f }, camRot);
}
 