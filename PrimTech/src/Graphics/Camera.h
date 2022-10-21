#pragma once
#include"Transform.h"
#include"Utility/COMException.h"
#include<string>
class Camera
{
public:
	Camera(std::string name = "default");
	void SetPerspective(float fovDeg, float aspectRatio, float nearZ, float farZ);
	void SetOrtographic(float width, float height, float nearZ, float farZ);
	void OverrideProjectionMatrix(const sm::Matrix& m);
	void OverrideViewMatrix(const sm::Matrix& m);

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

	std::string GetName() const;
	void SetName(const std::string& name);
private:
	std::string m_name;
	void UpdateView();
	sm::Vector3 m_position, m_rotation, m_offset;
	sm::Matrix m_viewM, m_projM;

	sm::Vector3 m_forwardV, m_leftV, m_upV;
	const float OFFSETMAX = 4.f;
	float m_rotationSpeed;
};

enum CamFlags
{
	CamFlags_eNONE = 0,
	CamFlags_eNO_DEFAULT_CAM,
	CamFlags_SAME_NAME_EQUALS_SAMECAM
};

class CameraHandler
{
public:
	CameraHandler();
	void Init(const d::XMINT2& resolutions, unsigned char flags = 0);
	Camera* CreatePerspectiveCamera(std::string name, float fovDeg, float aspectRatio, float nearZ, float farZ);
	Camera* CreateEmptyCamera(std::string name);
	Camera* GetCameraAdress(std::string name);
	Camera* GetCameraAdress(uint i);

	void RemoveCamera(std::string name);
	void RemoveCamera(uint i);

	Camera* SetCurrentCamera(uint i);
	Camera* SetCurrentCamera(std::string name);

	void ChangeCameraName(std::string oldName, std::string newName);

	uint GetNoOfCams() const;
	Camera* GetCurrentCamera() const;
	uint GetCurrentCamIndex() const;
	int GetCameraIndex(std::string name) const;
	std::string GetCameraName(uint i) const;
private:
	std::vector<Camera> m_cams;
	uint m_currentCamIndex = 0;
	Camera* mp_curentCam = nullptr;
	unsigned char m_flags = 0;
};

