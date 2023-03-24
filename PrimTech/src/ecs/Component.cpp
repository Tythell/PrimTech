#include "pch.h"
#include "Component.h"



namespace pt
{
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

	//MeshRef::MeshRef(const MeshRef& other): Component(other.EntId())
	//{
	//	m_pMesh = other.m_pMesh;
	//}

	void MeshRef::Init(std::string name)
	{
		if (name == "")
		{
			m_pMaterialindexes.resize(1, 0);
			return;
		}
		//std::vector<PrimtTech::Mesh>& meshArr = PrimtTech::ResourceHandler::GetMeshArrayReference();

		int meshIndex = PrimtTech::ResourceHandler::CheckMeshNameExists(name);

		m_meshIndex = (meshIndex == -1) ? 0 : meshIndex;

		uint u = PrimtTech::ResourceHandler::GetMeshAdress(m_meshIndex)->GetNofMeshes();
		m_pMaterialindexes.resize(u, 0);

	}

	void MeshRef::SetMaterial(uint materialIndex, uint slot)
	{
		m_pMaterialindexes[slot] = materialIndex;
	}

	void MeshRef::SetMaterial(std::string name, uint slot)
	{
		SetMaterial(PrimtTech::ResourceHandler::CheckMtrlNameExists(name), slot);
	}
	uint MeshRef::GetMaterialIndex(const uint& index) const
	{
		if (m_pMaterialindexes.size() == 0)
			return 0;
		return m_pMaterialindexes[index];
	}

	PhysicsBody::PhysicsBody(uint id) : Component(id)
	{
	}

	void PhysicsBody::Init(rp::RigidBody* pRigidBody)
	{
		mp_rigidBody = pRigidBody;
	}

	void PhysicsBody::UpdateTransform(pt::TransformComp& transform)
	{
		if (mp_rigidBody)
		{
			const reactphysics3d::Transform& rpTransform = mp_rigidBody->getTransform();

			rp::Vector3 rpPos(rpTransform.getPosition());
			rp::Quaternion rpQuat(rpTransform.getOrientation());
			sm::Vector3 smPos(rpPos.x, rpPos.y, rpPos.z);
			sm::Quaternion smQuat(rpQuat.x, rpQuat.y, rpQuat.z, rpQuat.w);

			transform.SetPosition(smPos);
			transform.SetRotation(smQuat);
		}
	}
	void PhysicsBody::SetType(rp::BodyType bodyType)
	{
		mp_rigidBody->setType(bodyType);
	}
}

