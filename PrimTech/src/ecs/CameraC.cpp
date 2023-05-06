#include "pch.h"
#include "CameraC.h"
#include "TransformC.h"

namespace pt
{
	pt::Camera::Camera(EntIdType entId) : Component(entId) {}

	sm::Matrix pt::Camera::GetViewMatrix() const
	{
		return m_viewM;
	}

	sm::Matrix pt::Camera::GetProjMatrix() const
	{
		return m_projM;
	}
	void Camera::SetProjMatrix(float* f)
	{
		m_projM = *reinterpret_cast<sm::Matrix*>(f);
		//m_projM = m_projM.Transpose();
		//memcpy(&m_projM.m[0][0], f, sizeof(sm::Matrix));
	}
	void Camera::SetViewMatrix(float* f)
	{
		m_viewM = *reinterpret_cast<sm::Matrix*>(f);
		//m_viewM = m_viewM.Transpose();
		//memcpy(&m_viewM.m[0][0], f, sizeof(sm::Matrix));
	}
	void Camera::SetPerspective(float fovDeg, float aspectRatio, float nearZ, float farZ)
	{
		float fovRad = (fovDeg / 360.f) * d::XM_2PI;
		m_projM = d::XMMatrixPerspectiveFovLH(fovRad, aspectRatio, nearZ, farZ);
		m_isOrthographic = false;
	}

	void Camera::SetOrtographic(float width, float height, float nearZ, float farZ)
	{
		m_projM = d::XMMatrixOrthographicLH(width, height, nearZ, farZ);
		m_isOrthographic = true;
	}
	void Camera::UpdateView(const pt::TransformComp& entTransform)
	{
		d::XMMATRIX camRot = d::XMMatrixRotationRollPitchYawFromVector(entTransform.GetRotation() + m_rotateOffset);

		d::XMVECTOR camTarget = d::XMVector3TransformCoord(sm::Vector4(0.f, 0.f, 1.f, 0.f), camRot);
		camTarget += entTransform.GetPosition();
		//camTarget += posOffset;

		d::XMVECTOR upDir = d::XMVector2TransformCoord(sm::Vector4(0.f, 1.f, 0.f, 0.f), camRot);

		m_viewM = d::XMMatrixLookAtLH(entTransform.GetPosition(), camTarget, upDir);

		m_viewM *= d::XMMatrixTranslationFromVector(m_posOffset);

		m_forwardV = d::XMVector3TransformCoord({ 0,0,1 }, camRot);
		m_leftV = d::XMVector3TransformCoord({ -1,0,0 }, camRot);
		m_upV = d::XMVector3TransformCoord({ 0.f,1.f,0.f }, camRot);
	}
	void Camera::SetPositionOffset(const sm::Vector3& v)
	{
		m_posOffset = v;
	}
	void Camera::SetPositionOffset(float x, float y, float z)
	{
		SetPositionOffset(sm::Vector3(x, y, z));
	}
	void Camera::SetRotationOffset(const sm::Vector3& v)
	{
		m_rotateOffset = v;
	}
	void Camera::SetRotationOffset(float x, float y, float z)
	{
		SetRotationOffset(sm::Vector3(x, y, z));
	}
	void Camera::DuplicateFrom(Component* other)
	{
		Camera* otherComp = dynamic_cast<Camera*>(other);
		m_viewM = otherComp->m_viewM; m_projM = otherComp->m_projM;
		m_posOffset = otherComp->m_posOffset; m_rotateOffset = otherComp->m_rotateOffset;
		m_forwardV = otherComp->m_forwardV;
		m_leftV = otherComp->m_leftV;
		m_upV = otherComp->m_upV;
		m_isOrthographic = otherComp->m_isOrthographic;
	}
	void Camera::OnFree()
	{
	}
}