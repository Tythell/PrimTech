#pragma once
#include"Transform.h"
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

	void Move(float x, float y, float z);
	void Move(const sm::Vector3 v);
	void Rotate(float x, float y, float z);
	void Rotate(const sm::Vector3 v);

	sm::Matrix GetViewM() const;
	sm::Matrix GetProjM() const;
private:
	void UpdateView();
	sm::Vector3 m_position, m_rotaion, m_offset;
	sm::Matrix m_viewM, m_projM;

	//sm::Vector3 m_forwardV, m_leftV;
};

