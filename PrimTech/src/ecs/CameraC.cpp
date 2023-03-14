#include "pch.h"
#include "Component.h"

namespace pt
{
	pt::CameraComp::CameraComp(uint entId) : Component(entId)
	{

	}

	sm::Matrix pt::CameraComp::GetViewMatrix() const
	{
		return m_viewM;
	}

	sm::Matrix pt::CameraComp::GetProjMatrix() const
	{
		return m_projM;
	}
	void CameraComp::SetPerspective(float fovDeg, float aspectRatio, float nearZ, float farZ)
	{
		float fovRad = (fovDeg / 360.f) * d::XM_2PI;
		m_projM = d::XMMatrixPerspectiveFovLH(fovRad, aspectRatio, nearZ, farZ);
	}

	void CameraComp::SetOrtographic(float width, float height, float nearZ, float farZ)
	{
		m_projM = d::XMMatrixOrthographicLH(width, height, nearZ, farZ);
	}
	void CameraComp::UpdateView(const pt::TransformComp& entTransform)
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
	void CameraComp::SetPositionOffset(const sm::Vector3& v)
	{
		m_posOffset = v;
	}
	void CameraComp::SetPositionOffset(float x, float y, float z)
	{
		SetPositionOffset(sm::Vector3(x, y, z));
	}
	void CameraComp::SetRotationOffset(const sm::Vector3& v)
	{
		m_rotateOffset = v;
	}
	void CameraComp::SetRotationOffset(float x, float y, float z)
	{
		SetRotationOffset(sm::Vector3(x, y, z));
	}
}