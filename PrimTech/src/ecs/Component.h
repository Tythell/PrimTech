#pragma once
#include "../Graphics/Mesh.h"
#include "../Graphics/Material.h"
#include "Graphics/CbufferTypes.h"
#undef max;
#undef min;
#include <reactphysics3d/reactphysics3d.h>
#include<string>

namespace rp = reactphysics3d;

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

		void Init(std::string path);
		void SetMaterial(uint materialIndex, uint slot = 0);
		void SetMaterial(std::string materialName, uint slot = 0);

		PrimtTech::Mesh* GetMeshContainerP() const { return PrimtTech::ResourceHandler::GetMeshAdress(m_meshIndex); }
		uint GetMaterialIndex(const uint& index) const;
		uint GetNumMaterials() const { return (uint)m_pMaterialindexes.size(); };
		std::string GetNameOfMesh() const { return PrimtTech::ResourceHandler::GetMeshAdress(m_meshIndex)->GetName(); }
	private:
		uint m_meshIndex = 0;
		std::vector<uint> m_pMaterialindexes;
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

	class Camera : public Component
	{
	public:
		Camera(uint entId);

		sm::Matrix GetViewMatrix() const;
		sm::Matrix GetProjMatrix() const;

		void SetPerspective(float fovDeg, float aspectRatio, float nearZ, float farZ);
		void SetOrtographic(float width, float height, float nearZ, float farZ);

		void UpdateView(const pt::TransformComp& entTransform);

		void SetPositionOffset(const sm::Vector3& v);
		void SetPositionOffset(float x, float y, float z);
		void SetRotationOffset(const sm::Vector3& v);
		void SetRotationOffset(float x, float y, float z);

		sm::Vector3 GetPositionOffset() const { return m_posOffset; }
		sm::Vector3 GetRotationOffset() const { return m_rotateOffset; }

		sm::Vector3 GetForwardV() const { return m_forwardV; };
		sm::Vector3 Getleft() const { 	return m_leftV; };
		sm::Vector3 GetUp() const { return m_upV; };
	private:
		sm::Matrix m_viewM, m_projM;
		sm::Vector3 m_posOffset, m_rotateOffset;
		sm::Vector3 m_forwardV, m_leftV, m_upV;

		bool m_isOrthographic = false;

	public:
		bool IsOrthograpgic() const { return m_isOrthographic; };
	};

	class AABBComp : public Component
	{
	public:
		AABBComp(uint entId);

		void Update(pt::TransformComp& transform);
		void SetExtends(const sm::Vector3& extends);
		void SetPositionOffset(const sm::Vector3& pos);
		sm::Vector3 GetPositionOffset() const;

		bool Intersects(AABBComp& box);
		void CreateFromMesh(const pt::MeshRef& mr);

		bool IsIntersecting() const { return m_isIntersecting; }
		void SetIsIntersecting(bool b) { m_isIntersecting = b; }

		d::BoundingBox GetBox() const { return m_aabb; };
	private:
		d::BoundingBox m_aabb;
		sm::Vector3 m_posOffset;
		bool m_isIntersecting = false;
	};

	class OBBComp : public Component
	{
	public:
		OBBComp(uint entId);

		void Update(const pt::TransformComp& transform);
		void SetExtends(const sm::Vector3& extends);
	private:
		d::BoundingOrientedBox m_obb;
	};

	class BSphereComp : public Component
	{
	public:
		BSphereComp(uint entId);

		void Update(const pt::TransformComp& transform);
		void SetRadius(float r);
	private:
		d::BoundingSphere m_bsphere;
	};

	class Light : public Component
	{
	public:
		Light(uint entId);

		void Update(const TransformComp& transform);
		void SetColor(sm::Vector4 clr) { m_lightData.clr = clr; };
		void SetOffsetPosition(sm::Vector4 pos) { memcpy(&m_posOffset.x, &pos.x, sizeof(float) * 3); };
		void SetDirectionOffset(const sm::Vector4& dir){ memcpy(&m_dirOffset.x, &dir.x, sizeof(float) * 3); }

		PrimtTech::hlsl::Light GetLightData() const { return m_lightData; };
		sm::Vector4 GetPositionOffset() const { return m_posOffset; };
		sm::Vector4 GetDirectionOffset() const { return m_dirOffset; };

		void SetType(uchar type);
		uchar GetType() const { return m_type; };
	private:
		PrimtTech::hlsl::Light m_lightData;
		sm::Vector4 m_posOffset;
		sm::Vector4 m_dirOffset;
		uchar m_type = 0u;
		//enum Type
		//{
		//	eLightType_Point,
		//	eLightType_Directional,
		//	eLightType_Ambient,
		//};
	};

	class PhysicsBody : public Component
	{
	public:
		PhysicsBody(uint id);

		//void AddColider(rp::, const pt::TransformComp& transform);
		void Init(rp::RigidBody* pRigidBody);
		void UpdateTransform(pt::TransformComp& transform);

		rp::BodyType GetType() const { return mp_rigidBody->getType(); }
		void SetType(rp::BodyType bodyType);
	private:
		reactphysics3d::RigidBody* mp_rigidBody = nullptr;
	};
}