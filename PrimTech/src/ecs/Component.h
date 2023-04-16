#pragma once
#include "../Graphics/Mesh.h"
#include "../Graphics/Material.h"
#include "Graphics/CbufferTypes.h"
#include "Physics/PhysicsHandler.h"
#include <lua/lua.hpp>
#include<string>

namespace rp = reactphysics3d;
typedef int EntIdType;

namespace pt
{
	class Component
	{
	public:
		Component(EntIdType id);
		const EntIdType EntId() const;
		void FreeComponent() { m_entId = -1; }
	private:
		EntIdType m_entId;
	};

	class MeshRef : public Component
	{
	public:
		MeshRef(EntIdType entId, std::string meshName = "");

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

	class TransformComp;

	class Camera : public Component
	{
	public:
		Camera(EntIdType entId);

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

		bool IsOrthograpgic() const { return m_isOrthographic; };
	private:
		sm::Matrix m_viewM, m_projM;
		sm::Vector3 m_posOffset, m_rotateOffset;
		sm::Vector3 m_forwardV, m_leftV, m_upV;

		bool m_isOrthographic = false;
	};

	class AABBComp : public Component
	{
	public:
		AABBComp(EntIdType entId);

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
		OBBComp(EntIdType entId);

		void Update(const pt::TransformComp& transform);
		void SetExtends(const sm::Vector3& extends);
	private:
		d::BoundingOrientedBox m_obb;
	};

	class BSphereComp : public Component
	{
	public:
		BSphereComp(EntIdType entId);

		void Update(const pt::TransformComp& transform);
		void SetRadius(float r);
	private:
		d::BoundingSphere m_bsphere;
	};

	class Light : public Component
	{
	public:
		Light(EntIdType entId);

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
		PhysicsBody(EntIdType id);

		enum class ePT_ShapeType
		{
			Box, Sphere, Capsule
		};

		//void AddColider(rp::, const pt::TransformComp& transform);
		void UpdateTransform(pt::TransformComp& transform);

		void CreateRigidBody(const pt::TransformComp& ptTransform);

		void AddBoxColider(const sm::Vector3& extents = { .5f, .5f, .5f});
		void AddSphereColider(float radius);
		void AddCapsuleColider(float radius, float height);
		void RemoveCollider(uint index = 0);

		rp::BodyType GetType() const { return mp_rigidBody->getType(); }
		void SetType(rp::BodyType bodyType);
		void SetPhysicsPosition(const sm::Vector3& v);
		void SetBoxExtents(const sm::Vector3& extents, uint index = 0);
		void SetSphereRadius(float r, uint index = 0);
		void SetCapsuleLengths(float r, float h, uint index = 0);
		void SetIsTrigger(bool trigger, uint i) { mp_rigidBody->getCollider(i)->setIsTrigger(trigger); }
		bool GetIsTrigger(uint i) const { return mp_rigidBody->getCollider(i)->getIsTrigger(); }

		ePT_ShapeType GetColliderType(uint index = 0) const { return m_shapeIndexes[index]; }

		sm::Vector3 GetExtents(uint index = 0) const;
		float GetSphereRadius(uint index = 0) const;
		sm::Vector2 GetCapsuleLengths(uint index = 0) const; // x = r, y = h
		uint GetNoColliders() const;
		bool Exists() const
		{
			return (mp_rigidBody);
		}

		//void ResetPosition()
		//{
		//	rp::Transform rpTransform;
		//	rpTransform.setPosition({ m_startPos.x,m_startPos.y,m_startPos.z });
		//	rpTransform.setOrientation({
		//		m_startOrientation.x,
		//		m_startOrientation.y,
		//		m_startOrientation.z,
		//		m_startOrientation.w });

		//	mp_rigidBody->setTransform(rpTransform);
		//}
		static void SetPtrs(PrimtTech::PhysicsHandler* pPhysHandle) { m_pPhysHandle = pPhysHandle; }
	private:
		reactphysics3d::RigidBody* mp_rigidBody = nullptr;
		static PrimtTech::PhysicsHandler* m_pPhysHandle;
		std::vector<ePT_ShapeType> m_shapeIndexes;
		sm::Vector3 m_startPos;
		sm::Quaternion m_startOrientation;
	};

	class LuaScript : public Component
	{
	public:
		LuaScript(EntIdType id) : Component(id) {}
		LuaScript(const LuaScript& other);
		~LuaScript();

		void LoadScript(lua_State* L, const char* scriptFile/*, pt::Entity* pEnt*/);
		void SetBuffer(lua_State* L, char* buffer, int size);

		void Execute(const char* funcName);

		int GetBufferSize() const { return m_size; };
		char* GetBufferP() const { return m_pbuffer; };;
	private:
		lua_State* L = nullptr;
		char* m_pbuffer = nullptr;;
		int m_size = -1;
	};
}