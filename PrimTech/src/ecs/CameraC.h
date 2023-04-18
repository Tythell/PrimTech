#pragma once
#include "Component.h"


namespace pt
{
	class TransformComp;

	class Camera : public Component
	{
	public:
		Camera(EntIdType entId);

		sm::Matrix GetViewMatrix() const;
		sm::Matrix GetProjMatrix() const;

		void SetPerspective(float fovDeg, float aspectRatio, float nearZ, float farZ);
		void SetOrtographic(float width, float height, float nearZ, float farZ);

		void UpdateView(const pt::TransformComp& entTransform);

		void SetPositionOffset(const sm::Vector3& v);
		void SetPositionOffset(float x, float y, float z);
		void SetRotationOffset(const sm::Vector3& v);
		void SetRotationOffset(float x, float y, float z);

		sm::Vector3 GetPositionOffset() const { return m_posOffset; }
		sm::Vector3 GetRotationOffset() const { return m_rotateOffset; }

		sm::Vector3 GetForwardV() const { return m_forwardV; };
		sm::Vector3 Getleft() const { return m_leftV; };
		sm::Vector3 GetUp() const { return m_upV; };

		bool IsOrthograpgic() const { return m_isOrthographic; };
	private:
		sm::Matrix m_viewM, m_projM;
		sm::Vector3 m_posOffset, m_rotateOffset;
		sm::Vector3 m_forwardV, m_leftV, m_upV;

		bool m_isOrthographic = false;
	};
}

