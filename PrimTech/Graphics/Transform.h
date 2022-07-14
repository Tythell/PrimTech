#pragma once
#include"../Math/Math.h"
class Transform
{
public:
	Transform();
	void SetPosition(float x, float y, float z);
	void SetPosition(sm::Vector3 v);
	void SetRotation(float x, float y, float z);
	void SetRotation(sm::Vector3 v);
	void SetScale(float x, float y, float z);
	void SetScale(float xyz);
	void SetScale(sm::Vector3 v);
	void Move(float x, float y, float z);
	void Move(sm::Vector3 v);
	void Rotate(float x, float y, float z);
	void Rotate(sm::Vector3 v);
	void Scale(float x, float y, float z);
	void Scale(sm::Vector3 v);
	void Scale(float xyz);
	sm::Vector3 GetPosition() const;
	sm::Vector3 GetRotation() const;
	sm::Vector3 GetScale() const;
protected:
	sm::Matrix GetWorldTransposed();
	void UpdateWorld();
private:
	sm::Vector3 m_pos, m_rot, m_scale;
	sm::Matrix worldTransposed;
};

void ForceRotation(sm::Vector3& v);
void ForceRotation(float& x, float& y, float& z);
