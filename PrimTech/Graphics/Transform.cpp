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
	if (x < 0.f || x > d::XM_2PI) x = d::XM_2PI + x;
	if (y < 0.f || y > d::XM_2PI) y = d::XM_2PI + y;
	if (z < 0.f || z > d::XM_2PI) z = d::XM_2PI + z;
	m_rot = sm::Vector3(x, y, z);
	UpdateWorld();
}

void Transform::SetRotation(sm::Vector3 v)
{
	if (v.x < 0.f || v.x > d::XM_2PI) v.x = d::XM_2PI + v.x;
	if (v.y < 0.f || v.y > d::XM_2PI) v.y = d::XM_2PI + v.y;
	if (v.z < 0.f || v.z > d::XM_2PI) v.z = d::XM_2PI + v.z;
	m_rot = v;
	UpdateWorld();
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
	m_rot += sm::Vector3(x, y, z);
	//if (m_rot.x < 0.f || m_rot.x > d::XM_2PI) m_rot.x = d::XM_2PI + m_rot.x;
	//if (m_rot.y < 0.f || m_rot.y > d::XM_2PI) m_rot.y = d::XM_2PI + m_rot.y;
	//if (m_rot.z < 0.f || m_rot.z > d::XM_2PI) m_rot.z = d::XM_2PI + m_rot.z;
	ForceRotation(m_rot);
	UpdateWorld();
}

void Transform::Rotate(sm::Vector3 v)
{
	m_rot += v;
	//if (m_rot.x < 0.f || m_rot.x > d::XM_2PI) m_rot.x = d::XM_2PI + m_rot.x;
	//if (m_rot.y < 0.f || m_rot.y > d::XM_2PI) m_rot.y = d::XM_2PI + m_rot.y;
	//if (m_rot.z < 0.f || m_rot.z > d::XM_2PI) m_rot.z = d::XM_2PI + m_rot.z;
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

void ForceRotation(sm::Vector3& v)
{
	if (v.x < 0.f || v.x > d::XM_2PI) v.x += d::XM_2PI;
	if (v.y < 0.f || v.y > d::XM_2PI) v.y += d::XM_2PI;
	if (v.z < 0.f || v.z > d::XM_2PI) v.z += d::XM_2PI;
}

void ForceRotation(float& x, float& y, float& z)
{
	if (x < 0.f || x > d::XM_2PI) x += d::XM_2PI;
	if (y < 0.f || y > d::XM_2PI) y += d::XM_2PI;
	if (z < 0.f || z > d::XM_2PI) z += d::XM_2PI;
}
