#include "pch.h"
#include "Component.h"
#include "TransformC.h"
#include "ComponentHandler.h"

namespace pt
{
	TransformComp::TransformComp(EntIdType entId) : Component(entId),
		m_scale(1.f, 1.f, 1.f), m_pos(0.f,0.f,0.f), m_rotQ(0.f,0.f,0.f, 1.f)
	{
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

	void TransformComp::SetRotation(float x, float y, float z)
	{
		SetRotation(float3(x, y, z));
		UpdateWorld();
	}

	void TransformComp::SetRotation(float3 v)
	{
		m_rotQ = glm::quat(v);

		UpdateWorld();
	}

	void TransformComp::SetRotation(quat q)
	{
		m_rotQ = q;
		UpdateWorld();
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
		UpdateWorld();
	}

	void TransformComp::Rotate(float3 v)
	{
		//quat quat = quat.CreateFromYawPitchRoll(v.x, v.y, v.z);

		//m_rotQ.

		m_rotQ = quat(glm::eulerAngles(m_rotQ) + v);
		//m_rotQ = d::XMQuaternionRotationRollPitchYawFromVector(v+ glm::eulerAngles(m_rotQ.ToEuler());
		//ptm::ForceRotation(m_rot);

		UpdateWorld();
	}

	void TransformComp::Scale(float x, float y, float z)
	{
		m_scale += float3(x, y, z);
		UpdateWorld();
	}

	void TransformComp::Scale(float3 v)
	{
		m_scale += v;
		UpdateWorld();
	}

	void TransformComp::Scale(float xyz)
	{
		m_scale += float3(xyz, xyz, xyz);
		UpdateWorld();
	}

	void TransformComp::SetWorldMatrix(matrix m)
	{
		worldTransposed = glm::transpose(m);
	}

	float3 TransformComp::GetPosition() const
	{
		return m_pos;
	}

	float3 TransformComp::GetRotation() const
	{
		//float3 eulerRotation = m_rot.ToEuler();
		//return eulerRotation;
		return glm::eulerAngles(m_rotQ);
	}

	float3 TransformComp::GetScale() const
	{
		return m_scale;
	}

	void TransformComp::UpdateWorld()
	{
		//worldTransposed =
		//	d::XMMatrixScalingFromVector(m_scale) *
		//	d::XMMatrixRotationQuaternion(m_rotQ) *
		//	d::XMMatrixTranslationFromVector(m_pos);
		//worldTransposed = worldTransposed.Transpose();
		/*worldTransposed = */
		worldTransposed = glm::scale(worldTransposed, m_scale);
		worldTransposed *= glm::toMat4(m_rotQ);
		worldTransposed = glm::translate(worldTransposed, m_pos);
	}

	void TransformComp::OnFree() {}

	void TransformComp::DuplicateFrom(Component* other)
	{
		TransformComp* otherComp = dynamic_cast<TransformComp*>(other);
		m_pos = otherComp->m_pos;
		m_rotQ = otherComp->m_rotQ;
		m_scale = otherComp->m_scale;
		worldTransposed = otherComp->worldTransposed;
	}

	matrix TransformComp::GetWorldTransposed() const
	{
		return worldTransposed;
	}

	matrix TransformComp::GetWorld() const
	{
		matrix world = worldTransposed;
		return glm::transpose(worldTransposed);
	}

	matrix TransformComp::GetWorldInversed()
	{
		matrix matrix =
			glm::inverse(glm::scale(worldTransposed, m_scale)) *
			glm::inverse(glm::toMat4(m_rotQ)) *
			glm::inverse(glm::scale(worldTransposed, m_scale));
		return matrix;
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

