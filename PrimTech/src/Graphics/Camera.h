#pragma once
#include"Transform.h"
#include<string>
class Camera
{
public:
	Camera();
	void SetPerspective(float fovDeg, float aspectRatio, float nearZ, float farZ);
	void SetOrtographic(float width, float height, float nearZ, float farZ);

	void SetPosition(float x, float y, float z);
	void SetPosition(const sm::Vector3 v);
	void SetRotation(float x, float y, float z);
	void SetRotation(const sm::Vector3 v);
	void SetOffset(float x, float y, float z);
	void SetOffset(const sm::Vector3& v);

	void Move(float x, float y, float z);
	void Move(const sm::Vector3 v);
	void Rotate(float x, float y, float z);
	void Rotate(const sm::Vector3 v);
	void Offset(float x, float y, float z);
	void Offset(const sm::Vector3 v);

	sm::Matrix GetViewM() const;
	sm::Matrix GetProjM() const;

	sm::Vector3 GetForwardVector() const;
	sm::Vector3 GetForwardVectorNoY() const;
	sm::Vector3 GetLeftVector() const;
	sm::Vector3 GetUpVector() const;
	sm::Vector3 GetOffset() const;
	sm::Vector3 GetRelativeOffset() const;
	sm::Vector3 GetPositionNoOffset() const;
	sm::Vector3 GetPosition() const;
	sm::Vector3 GetPositionXZ() const;
	sm::Vector3 GetRotation() const;;

	void SetRotationSpeed(float f);
private:
	void UpdateView();
	sm::Vector3 m_position, m_rotation, m_offset;
	sm::Matrix m_viewM, m_projM;

	sm::Vector3 m_forwardV, m_leftV, m_upV;
	const float OFFSETMAX = 4.f;
	float m_rotationSpeed;
};

