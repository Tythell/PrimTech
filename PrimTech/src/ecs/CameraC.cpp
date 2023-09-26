#include "pch.h"
#include "CameraC.h"
#include "TransformC.h"

namespace pt
{
	pt::Camera::Camera(EntIdType entId) : Component(entId),
		m_forwardV(0.f,0.f,1.f), m_leftV(0.f, 0.f, 1.f), m_upV(0.f,1.f,0.f),
		m_projM(1.f), m_viewM(1.f),
		m_rotateOffset(0.f), m_posOffset(0.f, 0.f, -2.f)
	{}

	matrix pt::Camera::GetViewMatrix() const
	{
		return m_viewM;
	}

	matrix pt::Camera::GetProjMatrix() const
	{
		return m_projM;
	}
	void Camera::SetProjMatrix(float* f)
	{
		m_projM = *reinterpret_cast<matrix*>(f);
		//m_projM = m_projM.Transpose();
		//memcpy(&m_projM.m[0][0], f, sizeof(matrix));
	}
	void Camera::SetViewMatrix(float* f)
	{
		m_viewM = *reinterpret_cast<matrix*>(f);
		//m_viewM = m_viewM.Transpose();
		//memcpy(&m_viewM.m[0][0], f, sizeof(matrix));
	}
	void Camera::SetPerspective(float fovDeg, float width, float height, float nearZ, float farZ)
	{
		float fovRad = glm::radians(fovDeg);
		m_projM = glm::perspectiveFovLH(fovRad, width, height, nearZ, farZ);
		//m_projM = d::XMMatrixPerspectiveFovLH(fovRad, aspectRatio, nearZ, farZ);
		m_isOrthographic = false;
	}

	void Camera::SetOrtographic(float width, float height, float nearZ, float farZ)
	{
		m_projM = glm::orthoLH(-width * 0.5f, width * 0.5f, -height * 0.5f, height * 0.5f, nearZ, farZ);
		//m_projM = d::XMMatrixOrthographicLH(width, height, nearZ, farZ);
		m_isOrthographic = true;
	}
	void Camera::UpdateView(const pt::TransformComp& entTransform)
	{
		matrix camRot = glm::inverse(glm::toMat4(entTransform.GetRotationQuaternion()));

		

		float3 rotAngles = glm::degrees(glm::eulerAngles(entTransform.GetRotationQuaternion()));

		float4 camTarget = float4(0.f, 0.f, 1.f, 0.f) * camRot;

		camTarget = camTarget + float4(entTransform.GetPosition(), 0.f);

		float4 upDir = float4(0.f, 1.f, 0.f, 0.f) * camRot;

		m_viewM = glm::lookAtLH(entTransform.GetPosition(), float3(camTarget), float3(upDir));

		//d::XMVECTOR camTarget = d::XMVector3TransformCoord(float4(0.f, 0.f, 1.f, 0.f), camRot);
		//camTarget += entTransform.GetPosition();
		////camTarget += posOffset;

		//d::XMVECTOR upDir = d::XMVector2TransformCoord(float4(0.f, 1.f, 0.f, 0.f), camRot);

		//m_viewM = d::XMMatrixLookAtLH(entTransform.GetPosition(), camTarget, upDir);

		//m_viewM *= d::XMMatrixTranslationFromVector(m_posOffset);

		//m_forwardV = d::XMVector3TransformCoord({ 0,0,1 }, camRot);
		//m_leftV = d::XMVector3TransformCoord({ -1,0,0 }, camRot);
		//m_upV = d::XMVector3TransformCoord({ 0.f,1.f,0.f }, camRot);
		m_forwardV = float4(0.f, 0.f, 1.f, 0.f) * camRot;;
		m_leftV = float4(-1.f, 0.f, 0.f, 0.f) * camRot;
		m_upV = upDir;
	}
	void Camera::SetPositionOffset(const float3& v)
	{
		m_posOffset = v;
	}
	void Camera::SetPositionOffset(float x, float y, float z)
	{
		SetPositionOffset(float3(x, y, z));
	}
	void Camera::SetRotationOffset(const float3& v)
	{
		m_rotateOffset = v;
	}
	void Camera::SetRotationOffset(float x, float y, float z)
	{
		SetRotationOffset(float3(x, y, z));
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