#pragma once
#include "Component.h"

namespace pt
{
	class TransformComp : public Component
	{
	public:
		TransformComp(EntIdType entId);
		void SetPosition(float x, float y, float z);
		void SetPosition(float3 v);
		void SetRotation(float x, float y, float z);
		void SetRotation(float3 v);
		// Will mess up display angles
		void SetRotationQ(quat q);
		void SetRotationDeg(float x, float y, float z);
		void SetRotationDeg(float3 v);

		void SetScale(float x, float y, float z);
		void SetScale(float xyz);
		void SetScale(float3 v);
		void Move(float x, float y, float z);
		void Move(float3 v);
		void Rotate(float x, float y, float z);
		void Rotate(float3 v);
		void Scale(float x, float y, float z);
		void Scale(float3 v);
		void Scale(float xyz);

		void SetWorldMatrix(matrix m);

		float3 GetPosition() const;
		float3 GetRotationDeg() const;
		float3 GetScale() const;
		matrix GetWorld() const;
		matrix GetWorldTranspose() const;
		matrix GetWorldInversed();
		quat GetRotationQuaternion() const;

		//static int Lua_Move(lua_State* L);
		// Inherited via Component
		virtual void DuplicateFrom(Component* other) override;
		// Inherited via Component
		virtual void OnFree() override;
	protected:
		void UpdateWorld();
	private:
		float3 m_pos, m_scale, m_anglesD;
		quat m_rotQ;
		matrix worldM;
	};
}
