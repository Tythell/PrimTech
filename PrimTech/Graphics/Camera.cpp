#include "Camera.h"

Camera::Camera():
	m_position(0,0,0), m_rotation(0,0,0), m_offset(0,0,0)
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
	m_rotation = sm::Vector3(x,y,z);
	//if (m_rotation.x < 0.f || m_rotation.x > d::XM_2PI) m_rotation.x = d::XM_2PI + m_rotation.x;
	//if (m_rotation.y < 0.f || m_rotation.y > d::XM_2PI) m_rotation.y = d::XM_2PI + m_rotation.y;
	//if (m_rotation.z < 0.f || m_rotation.z > d::XM_2PI) m_rotation.z = d::XM_2PI + m_rotation.z;
	ForceRotation(m_rotation);
	UpdateView();
}

void Camera::SetRotation(const sm::Vector3 v)
{
	m_rotation = v;
	//if (m_rotation.x < 0.f || m_rotation.x > d::XM_2PI) m_rotation.x = d::XM_2PI + m_rotation.x;
	//if (m_rotation.y < 0.f || m_rotation.y > d::XM_2PI) m_rotation.y = d::XM_2PI + m_rotation.y;
	//if (m_rotation.z < 0.f || m_rotation.z > d::XM_2PI) m_rotation.z = d::XM_2PI + m_rotation.z;
	ForceRotation(m_rotation);
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
	m_rotation += sm::Vector3(x, y, z) * (m_rotationSpeed);
	//if (m_rotation.x > 0.f) m_rotation.x = d::XM_2PI;
	
	ForceRotation(m_rotation);
	//if (m_rotation.x < 0.f || m_rotation.x > d::XM_2PI) m_rotation.x = d::XM_2PI - m_rotation.x;
	//if (m_rotation.y < 0.f || m_rotation.y > d::XM_2PI) m_rotation.y = d::XM_2PI - m_rotation.y;
	//if (m_rotation.z < 0.f || m_rotation.z > d::XM_2PI) m_rotation.z = d::XM_2PI - m_rotation.z;
	UpdateView();
}

void Camera::Rotate(const sm::Vector3 v)
{
	m_rotation += v * (m_rotationSpeed);
	//if (m_rotation.x < 0.f || m_rotation.x > d::XM_2PI) m_rotation.x = d::XM_2PI - m_rotation.x;
	//if (m_rotation.y < 0.f || m_rotation.y > d::XM_2PI) m_rotation.y = d::XM_2PI - m_rotation.y;
	//if (m_rotation.z < 0.f || m_rotation.z > d::XM_2PI) m_rotation.z = d::XM_2PI - m_rotation.z;
	ForceRotation(m_rotation);
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

sm::Vector3 Camera::GetRotation() const
{
	return m_rotation;
}

void Camera::SetRotationSpeed(float f)
{
	m_rotationSpeed = f;
}

void Camera::UpdateView()
{
	d::XMMATRIX camRot = d::XMMatrixRotationRollPitchYawFromVector(m_rotation);

	d::XMVECTOR camTarget = d::XMVector3TransformCoord(sm::Vector4(0.f,0.f,1.f, 0.f ), camRot);
	camTarget += m_position;

	d::XMVECTOR upDir = d::XMVector2TransformCoord(sm::Vector4(0.f, 1.f, 0.f, 0.f), camRot);

	//if (m_offset.z < 0.f) m_offset.z = 0.f;
	//else if (m_offset.z > OFFSETMAX) m_offset.z = OFFSETMAX;
	
	sm::Matrix offsetMatrix = d::XMMatrixTranslationFromVector(m_offset);
	m_viewM = d::XMMatrixLookAtLH(m_position, camTarget, upDir);


	m_viewM =  m_viewM * offsetMatrix;


	m_forwardV = d::XMVector3TransformCoord({ 0,0,1 }, camRot);
	m_leftV = d::XMVector3TransformCoord({ -1,0,0 }, camRot);
	m_upV = d::XMVector3TransformCoord({ 0.f,1.f,0.f }, camRot);
}
 