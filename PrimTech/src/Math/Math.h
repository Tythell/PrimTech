#pragma once
//#include<DirectXMath.h>
//#include <DirectXCollision.h>
//#include"../Math/Simplemath.h"
#define GLM_FORCE_XYZW_ONLY
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <cmath>
#include <string>
#include <cstdint>
#include <algorithm>
//namespace smm = DirectX::SimpleMath;
//namespace d = DirectX;

using uint = unsigned int;
using uint2 = glm::uvec2;
using uint3 = glm::uvec3;
using uint4 = glm::uvec4;

using float2 = glm::vec2;
using float3 = glm::vec3;
using float4 = glm::vec4;

using float4x4 = glm::mat4x4;
using matrix = glm::mat4x4;

using int2 = glm::ivec2;
using int3 = glm::ivec3;
using int4 = glm::ivec4;

using uchar = unsigned char;
using ulong = unsigned long;
using ullong = unsigned long long;

using quat = glm::quat;

//#define DEG(r) r * glm::pi / 180

namespace ptm
{
	inline int firstset(int bits) {
		return (bits & 0x80000000) ? 31 : firstset((bits << 1) | 1) - 1;
	}

	inline std::string GetVectorAsString(float3 v)
	{
		std::string s = std::to_string(v.x);
		s += " ";
		s += std::to_string(v.y);
		s += " ";
		s += std::to_string(v.z);
		return s;
	}
	inline std::string GetVectorAsString(float4 v)
	{
		std::string s = std::to_string(v.x);
		s += " ";
		s += std::to_string(v.y);
		s += " ";
		s += std::to_string(v.z);
		s += " ";
		s += std::to_string(v.z);
		return s;
	}

	inline float RandomNum(int n1, int n2, unsigned int decimals = 0)
	{
		int powD = 1;
		if (decimals != 0)
		{
			powD = (int)pow(decimals, 10);
			n1 *= powD;
			n2 *= powD;
		}
		if (n1 > n2)
		{
			int temp = n2;
			n2 = n1;
			n1 = temp;
		}

		return float((rand() % n2 + n1) / powD);
	}

	inline void ForceRotation(float3& v)
	{
		//v.x = fmodf(v.x, d::XM_2PI);
		//v.y = fmodf(v.y, d::XM_2PI);
		//v.z = fmodf(v.z, d::XM_2PI);

		//if (v.x < 0.f) v.x += d::XM_2PI;
		//if (v.y < 0.f) v.y += d::XM_2PI;
		//if (v.z < 0.f) v.z += d::XM_2PI;


		//else if (v.x > d::XM_2PI) v.x -= d::XM_2PI;
		//else if (v.y > d::XM_2PI) v.y -= d::XM_2PI;
		//else if (v.z > d::XM_2PI) v.z -= d::XM_2PI;
	}

	inline void ForceRotation(float& x, float& y, float& z)
	{
		float3 v(x, y, z);
		ForceRotation(v);
		x = v.x;
		y = v.y;
		z = v.z;
	}

	inline float GetHighestValue(const float3& v)
	{
		float arr[3] = { v.x, v.y, v.z };
		float value = -999999;
		for (int i = 0; i < 3; i++)
		{
			if (arr[i] > value) value = arr[i];
		}
		return value;
	}

	inline float GetAvarageValue(const float3& v)
	{
		return (v.x + v.y + v.z) / 3;
	}

	inline int RandomNum(int n1, int n2)
	{
		if (n2 < n1)
		{
			int temp = n2;
			n2 = n1;
			n1 = temp;
		}
		
		return rand() % n2 + n1;;
	}

	inline float RandomFloat(int n1, int n2, int decimals)
	{
		if (n2 < n1)
			std::swap(n1, n2);

		return (float)(rand() % n2 + n1) / (float)pow(10, decimals);
	}
}


