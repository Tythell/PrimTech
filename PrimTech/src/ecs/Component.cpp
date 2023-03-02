#include "Component.h"

Component::Component(const uint id) : m_entId(id)
{}

const uint Component::EntId() const
{
	return m_entId;
}

MeshRef::MeshRef(uint entId, std::string meshName) : Component(entId)
{
	Init(meshName);
}

MeshRef::MeshRef(const MeshRef& other): Component(other.EntId())
{
	m_pMesh = other.m_pMesh;
}

void MeshRef::Init(std::string name)
{
	if (name == "")
	{
		return;
	}
	std::vector<Mesh>& meshArr = ResourceHandler::GetMeshArrayReference();

	for (int i = 0; i < meshArr.size(); i++)
	{
		if (meshArr[i].GetName() == name)
		{
			m_pMesh = &meshArr[i];
			return;
		}
	}
	m_pMesh = &meshArr[0];
}

TransformComp::TransformComp(uint entId) : Component(entId),
	m_scale(1.f, 1.f, 1.f)
{
}

void TransformComp::SetPosition(float x, float y, float z)
{
	m_pos = sm::Vector3(x, y, z);
	UpdateWorld();
}

void TransformComp::SetPosition(sm::Vector3 v)
{
	m_pos = v;
	UpdateWorld();
}

void TransformComp::SetRotation(float x, float y, float z)
{
	SetRotation(sm::Vector3(x, y, z));
	UpdateWorld();
}

void TransformComp::SetRotation(sm::Vector3 v)
{
	//if (v.x < 0.f || v.x > d::XM_2PI) v.x = d::XM_2PI + v.x;
	//if (v.y < 0.f || v.y > d::XM_2PI) v.y = d::XM_2PI + v.y;
	//if (v.z < 0.f || v.z > d::XM_2PI) v.z = d::XM_2PI + v.z;
	m_rot = v;
	ForceRotation(m_rot);
	//if (v.x + v.y + v.z != 0.f)
		//printf("");
	//sm::Quaternion quat = quat.CreateFromYawPitchRoll(v.x, v.y, v.z);

	//m_rot = quat;
	UpdateWorld();
}

void TransformComp::SetRotation(sm::Quaternion q)
{
	m_rot = q.ToEuler();
}

void TransformComp::SetScale(float x, float y, float z)
{
	m_scale = sm::Vector3(x, y, z);
	UpdateWorld();
}

void TransformComp::SetScale(float xyz)
{
	m_scale = sm::Vector3(xyz, xyz, xyz);
	UpdateWorld();
}

void TransformComp::SetScale(sm::Vector3 v)
{
	m_scale = v;
	UpdateWorld();
}

void TransformComp::Move(float x, float y, float z)
{
	m_pos += sm::Vector3(x, y, z);
	UpdateWorld();
}

void TransformComp::Move(sm::Vector3 v)
{
	m_pos += v;
	UpdateWorld();
}

void TransformComp::Rotate(float x, float y, float z)
{
	Rotate(sm::Vector3(x, y, z));
	UpdateWorld();
}

void TransformComp::Rotate(sm::Vector3 v)
{
	//sm::Quaternion quat = quat.CreateFromYawPitchRoll(v.x, v.y, v.z);
	m_rot += v;
	ForceRotation(m_rot);

	UpdateWorld();
}

void TransformComp::Scale(float x, float y, float z)
{
	m_scale += sm::Vector3(x, y, z);
	UpdateWorld();
}

void TransformComp::Scale(sm::Vector3 v)
{
	m_scale += v;
	UpdateWorld();
}

void TransformComp::Scale(float xyz)
{
	m_scale += sm::Vector3(xyz, xyz, xyz);
	UpdateWorld();
}

void TransformComp::SetWorldMatrix(sm::Matrix m)
{
	worldTransposed = m.Transpose();
}

sm::Vector3 TransformComp::GetPosition() const
{
	return m_pos;
}

sm::Vector3 TransformComp::GetRotation() const
{
	//sm::Vector3 eulerRotation = m_rot.ToEuler();
	//return eulerRotation;
	return m_rot;
}

sm::Vector3 TransformComp::GetScale() const
{
	return m_scale;
}

void TransformComp::UpdateWorld()
{
	worldTransposed =
		d::XMMatrixScalingFromVector(m_scale) *
		d::XMMatrixRotationRollPitchYawFromVector(m_rot) *
		d::XMMatrixTranslationFromVector(m_pos);
	worldTransposed = worldTransposed.Transpose();
}

sm::Matrix TransformComp::GetWorldTransposed()
{
	return worldTransposed;
}

sm::Matrix TransformComp::GetWorld()
{
	sm::Matrix world = worldTransposed;
	return world.Transpose();
}

sm::Matrix TransformComp::GetWorldInversed()
{
	sm::Matrix matrix =
		d::XMMatrixInverse(nullptr, DirectX::XMMatrixTranslationFromVector(m_pos)) *
		d::XMMatrixInverse(nullptr, DirectX::XMMatrixRotationRollPitchYawFromVector(m_rot)) *
		d::XMMatrixInverse(nullptr, DirectX::XMMatrixScalingFromVector(m_scale));
	return matrix;
}

sm::Quaternion TransformComp::GetRotationQuaternion() const
{
	sm::Quaternion quat = quat.CreateFromYawPitchRoll(m_rot.x, m_rot.y, m_rot.z);
	return quat;
}