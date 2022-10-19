#include "Transform.h"

Transform::Transform():
	m_scale(1.f,1.f,1.f)
{
}

void Transform::SetPosition(float x, float y, float z)
{
	m_pos = sm::Vector3(x, y, z);
	UpdateWorld();
}

void Transform::SetPosition(sm::Vector3 v)
{
	m_pos = v;
	UpdateWorld();
}

void Transform::SetRotation(float x, float y, float z)
{
	SetRotation(sm::Vector3(x, y, z));
	UpdateWorld();
}

void Transform::SetRotation(sm::Vector3 v)
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

void Transform::SetRotation(sm::Quaternion q)
{
	m_rot = q.ToEuler();
}

void Transform::SetScale(float x, float y, float z)
{
	m_scale = sm::Vector3(x, y, z);
	UpdateWorld();
}

void Transform::SetScale(float xyz)
{
	m_scale = sm::Vector3(xyz, xyz, xyz);
	UpdateWorld();
}

void Transform::SetScale(sm::Vector3 v)
{
	m_scale = v;
	UpdateWorld();
}

void Transform::Move(float x, float y, float z)
{
	m_pos += sm::Vector3(x, y, z);
	UpdateWorld();
}

void Transform::Move(sm::Vector3 v)
{
	m_pos += v;
	UpdateWorld();
}

void Transform::Rotate(float x, float y, float z)
{
	Rotate(sm::Vector3(x, y, z));
	UpdateWorld();
}

void Transform::Rotate(sm::Vector3 v)
{
	//sm::Quaternion quat = quat.CreateFromYawPitchRoll(v.x, v.y, v.z);
	m_rot += v;
	ForceRotation(m_rot);

	UpdateWorld();
}

void Transform::Scale(float x, float y, float z)
{
	m_scale += sm::Vector3(x, y, z);
	UpdateWorld();
}

void Transform::Scale(sm::Vector3 v)
{
	m_scale += v;
	UpdateWorld();
}

void Transform::Scale(float xyz)
{
	m_scale += sm::Vector3(xyz, xyz, xyz);
	UpdateWorld();
}

void Transform::OverrideWorldMatrix(sm::Matrix m)
{
	worldTransposed = m.Transpose();
}

sm::Vector3 Transform::GetPosition() const
{
	return m_pos;
}

sm::Vector3 Transform::GetRotation() const
{
	//sm::Vector3 eulerRotation = m_rot.ToEuler();
	//return eulerRotation;
	return m_rot;
}

sm::Vector3 Transform::GetScale() const
{
	return m_scale;
}

void Transform::UpdateWorld()
{
	worldTransposed = 
		d::XMMatrixScalingFromVector(m_scale) *
		d::XMMatrixRotationRollPitchYawFromVector(m_rot) *
		d::XMMatrixTranslationFromVector(m_pos);
	worldTransposed = worldTransposed.Transpose();
}

sm::Matrix Transform::GetWorldTransposed()
{
	return worldTransposed;
}

sm::Matrix Transform::GetWorld()
{
	sm::Matrix world = worldTransposed;
	return world.Transpose();
}

sm::Matrix Transform::GetWorldInversed()
{
	sm::Matrix matrix =
		d::XMMatrixInverse(nullptr, DirectX::XMMatrixTranslationFromVector(m_pos)) *
		d::XMMatrixInverse(nullptr, DirectX::XMMatrixRotationRollPitchYawFromVector(m_rot)) *
		d::XMMatrixInverse(nullptr, DirectX::XMMatrixScalingFromVector(m_scale));
	return matrix;
}

sm::Quaternion Transform::GetRotationQuaternion() const
{
	sm::Quaternion quat = quat.CreateFromYawPitchRoll(m_rot.x, m_rot.y, m_rot.z);
	return quat;
}

void ForceRotation(float& x, float& y, float& z)
{
	sm::Vector3 v(x, y, z);
	ForceRotation(v);
	x = v.x;
	y = v.y;
	z = v.z;
}

void ForceRotation(sm::Vector3& v)
{
	v.x = fmodf(v.x, d::XM_2PI);
	v.y = fmodf(v.y, d::XM_2PI);
	v.z = fmodf(v.z, d::XM_2PI);

	if (v.x < 0.f) v.x += d::XM_2PI;
	if (v.y < 0.f) v.y += d::XM_2PI;
	if (v.z < 0.f) v.z += d::XM_2PI;
	//else if (v.x > d::XM_2PI) v.x -= d::XM_2PI;
	//else if (v.y > d::XM_2PI) v.y -= d::XM_2PI;
	//else if (v.z > d::XM_2PI) v.z -= d::XM_2PI;
}