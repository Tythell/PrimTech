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
		ec_pling = 0x6,
		ec_ring = 0x10,
		ec_07 = 0x20,
		ec_08 = 0x40,
		//ec_09 = 0x80,
		//ec_10 = 0x100, 
	};

#define NUM_COMPONENT_TYPES 3

#define COMP_TYPE(cTYPE, NAME, STAT, MID) STAT std::vector<cTYPE> MID## NAME;


#define COMPONENT_LIST(STAT, AFTER) \
COMP_TYPE(pt::MeshRef, s_meshrefs, STAT, AFTER) \
COMP_TYPE(pt::TransformComp, s_transforms, STAT, AFTER) \
COMP_TYPE(pt::CameraComp, s_cams, STAT, AFTER) \

#define LINK_TYPE_VEC(ELSE,cTYPE, NAME, ENUM) \
ELSE if constexpr (std::is_same_v<T, cTYPE>) { \
	ptvec = &NAME; \
	c = ENUM; \
} \

#define LINK_LIST \
LINK_TYPE_VEC(##, pt::MeshRef, s_meshrefs, ec_meshRef) \
LINK_TYPE_VEC(else, pt::TransformComp, s_transforms, ec_transform) \
LINK_TYPE_VEC(else, pt::CameraComp, s_cams, ec_cam) \

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

