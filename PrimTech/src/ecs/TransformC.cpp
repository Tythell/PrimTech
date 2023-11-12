#include "pch.h"
#include "Component.h"
#include "TransformC.h"
#include "ComponentHandler.h"

namespace pt
{
	TransformComp::TransformComp(EntIdType entId) : Component(entId), worldM(1.f),
		m_scale(1.f, 1.f, 1.f), m_pos(0.f,0.f,0.f), m_rotQ(1.f,0.f,0.f, 0.f), m_anglesD(0.f, 0.f, 0.f)
	{
		//m_angles = glm::eulerAngles(m_rotQ);
	}

	void TransformComp::SetPosition(float x, float y, float z)
	{
		SetPosition(float3(x, y, z));
	}

	void TransformComp::SetPosition(float3 v)
	{
		m_pos = v;
		UpdateWorld();
	}

	void TransformComp::SetRotation(float x, float y, float z) { SetRotation(float3(x, y, z)); }

	void TransformComp::SetRotation(float3 v)
	{
		m_anglesD = glm::degrees(v);

		m_rotQ = quat(v);

		UpdateWorld();
	}

	void TransformComp::SetRotationQ(quat q)
	{
		m_rotQ = q;

		m_anglesD = glm::degrees(glm::eulerAngles(q));

		UpdateWorld();
	}

	void TransformComp::SetRotationDeg(float x, float y, float z)
	{
		SetRotationDeg(float3(x, y, z));
	}

	void TransformComp::SetRotationDeg(float3 v)
	{
		SetRotation(glm::radians(v));
	}

	void TransformComp::SetScale(float x, float y, float z)
	{
		SetScale(float3(x, y, z));
	}

	void TransformComp::SetScale(float xyz)
	{
		m_scale = float3(xyz, xyz, xyz);
		UpdateWorld();
	}

	void TransformComp::SetScale(float3 v)
	{
		m_scale = v;
		UpdateWorld();
	}

	void TransformComp::Move(float x, float y, float z)
	{
		m_pos += float3(x, y, z);
		UpdateWorld();
	}

	void TransformComp::Move(float3 v)
	{
		m_pos += v;
		UpdateWorld();
	}

	void TransformComp::Rotate(float x, float y, float z)
	{
		Rotate(float3(x, y, z));
	}

	void TransformComp::Rotate(float3 v)
	{
		m_anglesD += glm::degrees(v);

		SetRotationDeg(m_anglesD);
	}

	void TransformComp::Scale(float x, float y, float z)
	{
		Scale(float3(x, y, z));
	}

	void TransformComp::Scale(float3 v)
	{
		m_scale += v;
		UpdateWorld();
	}

	void TransformComp::Scale(float xyz)
	{
		Scale(float3(xyz, xyz, xyz));
	}

	void TransformComp::SetWorldMatrix(matrix m)
	{
		worldM = m;
	}

	float3 TransformComp::GetPosition() const
	{
		return m_pos;
	}

	float3 TransformComp::GetRotationDeg() const
	{
		return m_anglesD;
	}

	float3 TransformComp::GetScale() const
	{
		return m_scale;
	}

	void TransformComp::UpdateWorld()
	{
		//float3 angles(0.f, glm::radians(45.f), 0.f);

		worldM =
			glm::translate(matrix(1.f), m_pos) *
			glm::toMat4(m_rotQ) *
			glm::scale(matrix(1.f), m_scale);
			
	}

	void TransformComp::OnFree() {}

	void TransformComp::DuplicateFrom(Component* other)
	{
		*this = *dynamic_cast<TransformComp*>(other);
	}

	matrix TransformComp::GetWorld() const
	{
		return worldM;
		//matrix worldTest(1.f);
		//worldTest = glm::translate(worldTest, float3(0.f, 10000.f, -10.f));
		//return worldTest;
	}

	matrix TransformComp::GetWorldTranspose() const
	{
		//matrix world = worldTransposed;
		//matrix worldTest(1.f);
		//worldTest = glm::translate(worldTest, float3(0.f, 10000.f, -10.f));
		//return worldTest;
		return glm::transpose(worldM);
		//return matrix(1.f);
	}

	matrix TransformComp::GetWorldInversed()
	{
		return glm::inverse(worldM);
	}

	quat TransformComp::GetRotationQuaternion() const
	{
		return m_rotQ;
	}
	//int TransformComp::Lua_Move(lua_State* L)
	//{
	//	TransformComp* comp = (TransformComp*)lua_touserdata(L, 1);
	//	comp->Move(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));

	//	return 0;
	//}
}

