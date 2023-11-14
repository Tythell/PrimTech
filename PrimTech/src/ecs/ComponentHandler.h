#pragma once
#include "../Math/Math.h"
//#include "Component.h"
#include "Components.hpp"


namespace PrimtTech
{

	enum HasComponent
	{
		ec_null = 0,
		ec_transform = 0x1,
		ec_meshRef = 0x2,
		ec_cam = 0x4,
		ec_aabb = 0x8,
		ec_prefab = 0x10,
		//ec_bsphere = 0x20,
		ec_light = 0x40,
		ec_rigidBodies = 0x80,
		ec_lua = 0x100, 
	};


#define NUM_COMPONENT_TYPES 10

#define COMP_TYPE(cTYPE, NAME, STAT, MID) STAT std::vector<cTYPE> MID## NAME;


#define COMPONENT_LIST(STAT, AFTER) \
COMP_TYPE(pt::MeshRef, s_meshrefs, STAT, AFTER) \
COMP_TYPE(pt::MeshPrefabRef, s_meshPrefabs, STAT, AFTER) \
COMP_TYPE(pt::TransformComp, s_transforms, STAT, AFTER) \
COMP_TYPE(pt::Camera, s_cams, STAT, AFTER) \
COMP_TYPE(pt::Light, s_lights, STAT, AFTER) \
COMP_TYPE(pt::PhysicsBody, s_rigidBodies, STAT, AFTER) \
COMP_TYPE(pt::LuaScript, s_luascripts, STAT, AFTER) \

#define LINK_TYPE_VEC(ELSE,cTYPE, NAME, ENUM) \
ELSE if constexpr (std::is_same_v<T, cTYPE>) { \
	ptvec = &NAME; \
	c = ENUM; \
} \

#define LINK_LIST \
LINK_TYPE_VEC(##, pt::MeshRef, s_meshrefs, ec_meshRef) \
LINK_TYPE_VEC(else, pt::MeshPrefabRef, s_meshPrefabs, ec_prefab) \
LINK_TYPE_VEC(else, pt::TransformComp, s_transforms, ec_transform) \
LINK_TYPE_VEC(else, pt::Camera, s_cams, ec_cam) \
LINK_TYPE_VEC(else, pt::Light, s_lights, ec_light) \
LINK_TYPE_VEC(else, pt::PhysicsBody, s_rigidBodies, ec_rigidBodies) \
LINK_TYPE_VEC(else, pt::LuaScript, s_luascripts, ec_lua) \

#define GETCOMPVEC(type, name) std::vector<type>& name = PrimtTech::ComponentHandler::GetComponentArray<type>();
#define GETCOMPNUMUSED(type, name) name = PrimtTech::ComponentHandler::GetNoOfUsedComponents<type>();

	class ComponentHandler
	{
	public:
		template<class T>
		static std::vector<T>& GetComponentArray()
		{
			HasComponent dummy = ec_null;
			std::vector<T>* tVec = nullptr;

			LinkVector(tVec, dummy);
			return *tVec;
		}

		template<class T>
		static T& GetComponentByIndex(uint index)
		{
			HasComponent dummy = ec_null;
			std::vector<T>* tVec = nullptr;

			LinkVector(tVec, dummy);
			return tVec[0][index];
		}

		template<class T>
		static void ReserveMemory(unsigned int n)
		{
			std::vector<T>* tVec = nullptr;

			HasComponent dummy = ec_null;
			LinkVector<T>(tVec, dummy);
			tVec->reserve(n);
		}

		static int GetNoFreeComponents(int index);
		template <class T>
		static int GetNoOfUsedComponents()
		{
			HasComponent compType = ec_null;
			std::vector<T>* tVec = nullptr;
			LinkVector<T>(tVec, compType);
			int index = ptm::firstset(compType);
			return (int)tVec->size() - freeComponents[index];
		}
		static void IncreaseFreeComponents(int index, int n);
		static void SetFreeComponents(int index, int n);
		static void ZeroFreeArray();

		template<class T>
		static void LinkVector(std::vector<T>*& ptvec, HasComponent& c)
		{
			LINK_LIST
		}
	private:
		COMPONENT_LIST(static, );

		static int freeComponents[NUM_COMPONENT_TYPES];
	};

}

