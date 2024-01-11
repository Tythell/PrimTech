#pragma once
#include "Component.h"


namespace pt
{
	class TransformComp;

	class Camera : public Component
	{
	public:
		Camera(EntIdType entId);

		matrix GetViewMatrix() const;
		matrix GetProjMatrix() const;

		void SetProjMatrix(float* f);
		void SetViewMatrix(float* f);

		void SetPerspective(float fovDeg, float width, float height, float nearZ, float farZ);
		void SetOrtographic(float width, float height, float nearZ, float farZ);
		void RecalculateProjection(uint width, uint height);

		void UpdateView(const pt::TransformComp& entTransform);

		void SetPositionOffset(const float3& v);
		void SetPositionOffset(float x, float y, float z);
		void SetRotationOffset(const float3& v);
		void SetRotationOffset(float x, float y, float z);
		void RotateOffset(float x, float y, float z);
		void RotateOffset(const float3& v);
		void MoveOffset(float x, float y, float z);
		void MoveOffset(const float3& v);

		float3 GetPositionOffset() const { return m_posOffset; }
		float3 GetRotationOffset() const { return m_rotateOffset; }

		float3 GetForwardV() const { return m_forwardV; };
		float3 Getleft() const { return m_leftV; };
		float3 GetUp() const { return m_upV; };

		bool IsOrthograpgic() const { return m_isOrthographic; };

		// Inherited via Component
		virtual void DuplicateFrom(Component* other) override;
		virtual void OnFree() override;
	private:
		matrix m_viewM, m_projM;
		float3 m_posOffset, m_rotateOffset;
		float3 m_forwardV, m_leftV, m_upV;

		static const UINT FOV = 80.f;

		bool m_isOrthographic = false;

	};
}

