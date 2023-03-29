#include "pch.h"
#include "PhysicsHandler.h"

namespace PrimtTech
{
	PhysicsHandler::PhysicsHandler()
	{
		rp::PhysicsWorld::WorldSettings rpSettings;

		m_pworld = m_physics.createPhysicsWorld(rpSettings);

	}

	rp::RigidBody* PhysicsHandler::CreateRigidBody(const rp::Transform& transform, rp::BodyType bodyType)
	{
		rp::RigidBody* pRigidBody = nullptr;
		rp::BoxShape* pSphereShape = nullptr;
		//rp::SphereShape* pSphereShape = nullptr;


		pRigidBody = m_pworld->createRigidBody(transform);
		//pSphereShape = m_physics.createSphereShape(1.f);
		pSphereShape = m_physics.createBoxShape({.5f,.5f,.5f});

		pRigidBody->addCollider(pSphereShape, rp::Transform::identity());

		pRigidBody->setType(bodyType);

		return pRigidBody;
	}

	sm::Vector3 rpToSm(const rp::Vector3& f)
	{
		return sm::Vector3(f.x, f.y, f.z);
	}
	void PhysicsHandler::Update(float dt)
	{
		if (m_pworld->getNbRigidBodies() != 0)
			m_pworld->update(dt);
#ifdef _DEBUG
		if (m_isDraw)
		{
			std::vector<PrimtTech::BBVertex> tris;
			std::vector<PrimtTech::BBVertex> lines;

			tris.resize(md_debugRenderer->getNbTriangles() * 3);
			lines.resize(md_debugRenderer->getNbLines() * 2);

			for (int i = 0; md_debugRenderer->getIsDebugItemDisplayed(rp::DebugRenderer::DebugItem::COLLISION_SHAPE) && i < tris.size(); i += 3)
			{
				sm::Vector3 clr(1.f, 0.f, 0.f);

				tris[i + 0].m_position = rpToSm(md_debugRenderer->getTrianglesArray()[i / 3].point1);
				tris[i + 0].m_color = clr;
				tris[i + 1].m_position = rpToSm(md_debugRenderer->getTrianglesArray()[i / 3].point2);
				tris[i + 1].m_color = clr;
				tris[i + 2].m_position = rpToSm(md_debugRenderer->getTrianglesArray()[i / 3].point3);
				tris[i + 2].m_color = clr;
			}

			if (tris.size()) debugTris.CreateVertexBuffer(device, tris.data(), tris.size(), NULL, eBufferFlags_IgnoreCreateTwice);
		}
#endif // _DEBUG
	}

#ifdef _DEBUG
	void PhysicsHandler::EnableDebugDrawing(ID3D11Device*& pDevice, bool b)
	{
		m_isDraw = b;
		device = pDevice;

		m_pworld->setIsDebugRenderingEnabled(m_isDraw);
		md_debugRenderer = &m_pworld->getDebugRenderer();

		md_debugRenderer->setIsDebugItemDisplayed(rp::DebugRenderer::DebugItem::COLLISION_SHAPE, true);
		//md_debugRenderer->setIsDebugItemDisplayed(rp::DebugRenderer::DebugItem::, true);
	}

	void PhysicsHandler::DrawBuffers(ID3D11DeviceContext*& dc)
	{
		uint offset = 0;
		//dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		dc->IASetVertexBuffers(0, 1, debugTris.GetReference(), debugTris.GetStrideP(), &offset);
		dc->Draw(debugTris.GetBufferSize(), 0);
	}
#endif // _DEBUG


}