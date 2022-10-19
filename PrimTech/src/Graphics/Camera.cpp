#include "Camera.h"

Camera::Camera(std::string name):
	m_position(0,0,0), m_rotation(0,0,0), m_offset(0,0,0), m_name(name)
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

void Camera::SetOffset(const sm::Vector3& v)
{
	m_offset = v;
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

	float dummy = 0.f;

	ForceRotation(dummy, m_rotation.y, m_rotation.z);

	if (m_rotation.x > d::XM_PIDIV2) m_rotation.x = d::XM_PIDIV2;
	else if (m_rotation.x < -d::XM_PIDIV2) m_rotation.x = -d::XM_PIDIV2;
	//{
	//	m_rotation.x = m_rotation.x - x - d::XM_PIDIV2;
	//}
		
	//if (m_rotation.x > 0.f) m_rotation.x = d::XM_2PI;
	
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

sm::Vector3 Camera::GetForwardVectorNoY() const
{
	sm::Vector3 v(m_forwardV.x, 0.f, m_forwardV.z);
	v.Normalize();
	return v;
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

sm::Vector3 Camera::GetRelativeOffset() const
{
	return d::XMVector3TransformCoord(m_offset, d::XMMatrixRotationRollPitchYawFromVector(m_rotation));
}

sm::Vector3 Camera::GetPositionNoOffset() const
{
	return m_position;
}

sm::Vector3 Camera::GetPosition() const
{
	return m_position - GetRelativeOffset();
}

sm::Vector3 Camera::GetPositionXZ() const
{
	sm::Vector3 v = m_position - GetRelativeOffset();
	v.y = 0;
	return v;
}

sm::Vector3 Camera::GetRotation() const
{
	return m_rotation;
}

void Camera::SetRotationSpeed(float f)
{
	m_rotationSpeed = f;
}

void Camera::OverrideView(const sm::Matrix& m)
{
	m_viewM = m;
}

void Camera::OverrideProjection(const sm::Matrix& m)
{
	m_projM = m;
}

void Camera::OverrideViewProj(const sm::Matrix& m)
{
	THROW_POPUP_ERRORF(false, "OverrideViewProj är inte supported än, fråga Christian");
}

std::string Camera::GetName() const
{
	return m_name;
}

void Camera::SetName(const std::string& name)
{
	m_name = name;
}

void Camera::UpdateView()
{
	d::XMMATRIX camRot = d::XMMatrixRotationRollPitchYawFromVector(m_rotation);

	d::XMVECTOR camTarget = d::XMVector3TransformCoord(sm::Vector4(0.f,0.f,1.f, 0.f ), camRot);
	camTarget += m_position;

	d::XMVECTOR upDir = d::XMVector2TransformCoord(sm::Vector4(0.f, 1.f, 0.f, 0.f), camRot);

	if (m_offset.z < 0.f) m_offset.z = 0.f;
	else if (m_offset.z > OFFSETMAX) m_offset.z = OFFSETMAX;
	
	sm::Matrix offsetMatrix = d::XMMatrixTranslationFromVector(m_offset);
	m_viewM = d::XMMatrixLookAtLH(m_position, camTarget, upDir);

	m_viewM *= offsetMatrix;


	m_forwardV = d::XMVector3TransformCoord({ 0,0,1 }, camRot);
	m_leftV = d::XMVector3TransformCoord({ -1,0,0 }, camRot);
	m_upV = d::XMVector3TransformCoord({ 0.f,1.f,0.f }, camRot);
}
CameraHandler::CameraHandler()
{}

void CameraHandler::Init(const d::XMINT2& resolutions, unsigned char flags)
{
	if (!(flags & eNO_DEFAULT_CAM))
	{
		Camera* pCam = CreatePerspectiveCamera("DefaultCamera", 80, (float)resolutions.x / (float)resolutions.y, 0.1f, 100.f);
		pCam->SetPosition(2.f, 0, -3.f);
		m_currentCamIndex = 0;
	}
}

Camera* CameraHandler::CreatePerspectiveCamera(std::string name, float fovDeg, float aspectRatio, float nearZ, float farZ)
{
	bool nameExists = false;
	for (int i = 0; i < m_cams.size(); i++)
	{
		if (m_cams[i].GetName() == name)
		{
			name.append("1");
			break;
		}
	}
	if (!nameExists)
	{
		Camera cam(name);
		cam.SetPerspective(fovDeg, aspectRatio, nearZ, farZ);
		m_cams.emplace_back(cam);
		mp_curentCam = &m_cams[m_currentCamIndex];
		return &m_cams[m_cams.size() - 1];
	}
	else
		CreatePerspectiveCamera(name, fovDeg, aspectRatio, nearZ, farZ);
	return nullptr;
}

Camera* CameraHandler::GetCameraAdress(std::string name)
{
	for (int i = 0; i < m_cams.size(); i++)
	{
		if (m_cams[i].GetName() == name);
		return &m_cams[i];
	}
	return nullptr;
}

Camera* CameraHandler::GetCameraAdress(uint i)
{
	return &m_cams[i];
}

Camera* CameraHandler::SetCurrentCamera(uint i)
{
	m_currentCamIndex = i;
	mp_curentCam = &m_cams[i];
	return &m_cams[i];
}

Camera* CameraHandler::SetCurrentCamera(std::string name)
{
	int index = GetCameraIndex(name);
	return SetCurrentCamera(index);
}

void CameraHandler::ChangeCameraName(std::string oldName, std::string newName)
{
	for (int i = 0; i < m_cams.size(); i++)
	{
		if (m_cams[i].GetName() == oldName)
		{
			m_cams[i].SetName(newName);
			break;
		}
	}
}

uint CameraHandler::GetNoOfCams() const
{
	return m_cams.size();
}

Camera* CameraHandler::GetCurrentCamera() const
{
	return mp_curentCam;
}

uint CameraHandler::GetCurrentCamIndex() const
{
	return m_currentCamIndex;
}

int CameraHandler::GetCameraIndex(std::string name) const
{
	for (int i = 0; i < m_cams.size(); i++)
	{
		std::string cameraName = m_cams[i].GetName();
		if (cameraName == name)
			return i;
	}
	return -1;
}

std::string CameraHandler::GetCameraName(uint i) const
{
	return m_cams[i].GetName();
}
