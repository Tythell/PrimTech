#pragma once
//#include "../Math/Math.h"
#include "Component.h"

namespace PrimtTech {

	enum HasComponent
	{
		ec_null = 0,
		ec_transform = 0x1,
		ec_meshRef = 0x2,
		ec_cam = 0x4,
		ec_aabb = 0x8,
		ec_obb = 0x10,
		ec_bsphere = 0x20,
		ec_light = 0x40,
		//ec_08 = 0x80,
		//ec_09 = 0x100, 
	};

#define NUM_COMPONENT_TYPES 3

#define COMP_TYPE(cTYPE, NAME, STAT, MID) STAT std::vector<cTYPE> MID## NAME;


#define COMPONENT_LIST(STAT, AFTER) \
COMP_TYPE(pt::MeshRef, s_meshrefs, STAT, AFTER) \
COMP_TYPE(pt::TransformComp, s_transforms, STAT, AFTER) \
COMP_TYPE(pt::Camera, s_cams, STAT, AFTER) \
COMP_TYPE(pt::AABBComp, s_aabbs, STAT, AFTER) \
COMP_TYPE(pt::OBBComp, s_obbs, STAT, AFTER) \
COMP_TYPE(pt::BSphereComp, s_bspheres, STAT, AFTER) \
COMP_TYPE(pt::Light, s_lights, STAT, AFTER) \

#define LINK_TYPE_VEC(ELSE,cTYPE, NAME, ENUM) \
ELSE if constexpr (std::is_same_v<T, cTYPE>) { \
	ptvec = &NAME; \
	c = ENUM; \
} \

#define LINK_LIST \
LINK_TYPE_VEC(##, pt::MeshRef, s_meshrefs, ec_meshRef) \
LINK_TYPE_VEC(else, pt::TransformComp, s_transforms, ec_transform) \
LINK_TYPE_VEC(else, pt::Camera, s_cams, ec_cam) \
LINK_TYPE_VEC(else, pt::AABBComp, s_aabbs, ec_aabb) \
LINK_TYPE_VEC(else, pt::OBBComp, s_obbs, ec_obb) \
LINK_TYPE_VEC(else, pt::BSphereComp, s_bspheres, ec_bsphere) \
LINK_TYPE_VEC(else, pt::Light, s_lights, ec_light) \

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

		template<class T>
		static void LinkVector(std::vector<T>*& ptvec, HasComponent& c)
		{
			LINK_LIST
		}
	private:
		COMPONENT_LIST(static, )

	};

}

