#pragma once
#include "../Graphics/Mesh.h"
#include "../Graphics/Material.h"
#include<string>

namespace pt
{
	class Component
	{
	public:
		Component(const uint id);
		const uint EntId() const;

	private:
		const uint m_entId;
	};

	class MeshRef : public Component
	{
	public:
		MeshRef(uint entId, std::string meshName = "");
		//MeshRef(const MeshRef& other);

		void Init(std::string path);
		void SetMaterial(uint materialIndex, uint slot = 0);
		void SetMaterial(std::string materialName, uint slot = 0);

		PrimtTech::Mesh* GetMeshContainerP() const { return m_pMesh; }
		uint GetMaterialIndex(const uint& index) const;
		std::string GetNameOfMesh() const { return m_pMesh->GetName(); }
	private:
		std::vector<uint> m_pMaterialindexes;
		PrimtTech::Mesh* m_pMesh = &PrimtTech::ResourceHandler::GetMesh(0);

		//friend class PrimtTech::DX11Renderer;
	};


	class TransformComp : public Component
	{
	public:
		TransformComp(uint entId);
		void SetPosition(float x, float y, float z);
		void SetPosition(sm::Vector3 v);
		void SetRotation(float x, float y, float z);
		void SetRotation(sm::Vector3 v);
		void SetRotation(sm::Quaternion q);
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

		void SetWorldMatrix(sm::Matrix m);

		sm::Vector3 GetPosition() const;
		sm::Vector3 GetRotation() const;
		sm::Vector3 GetScale() const;
		sm::Matrix GetWorldTransposed() const;
		sm::Matrix GetWorld() const;
		sm::Matrix GetWorldInversed();
		sm::Quaternion GetRotationQuaternion() const;
	protected:
		void UpdateWorld();
	private:
		sm::Vector3 m_pos, m_rot, m_scale;
		//sm::Quaternion m_rot;
		sm::Matrix worldTransposed;
	};

	class CameraComp : public Component
	{
	public:
		CameraComp(uint entId);

		sm::Matrix GetViewMatrix() const;
		sm::Matrix GetProjMatrix() const;

		void SetPerspective(float fovDeg, float aspectRatio, float nearZ, float farZ);
		void SetOrtographic(float width, float height, float nearZ, float farZ);

		void UpdateView(const pt::TransformComp& entTransform);

		void SetPositionOffset(const sm::Vector3& v);
		void SetRotationOffset(const sm::Vector3& v);

		sm::Vector3 GetPositionOffset() const { return posOffset; }
		sm::Vector3 GetRotationOffset() const { return rotateOffset; }

		sm::Vector3 GetForwardV() const { return m_forwardV; };
		sm::Vector3 Getleft() const { 	return m_leftV; };
		sm::Vector3 GetUp() const { return m_upV; };
	private:
		sm::Matrix m_viewM, m_projM;

		sm::Vector3 posOffset, rotateOffset;

		sm::Vector3 m_forwardV, m_leftV, m_upV;
	};
}

