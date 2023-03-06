#pragma once
#include<DirectXMath.h>
#include <DirectXCollision.h>
#include"../Math/Simplemath.h"
#include <cmath>
#include <string>
#include <cstdint>
namespace sm = DirectX::SimpleMath;
namespace d = DirectX;

using uint = unsigned int;

namespace ptm
{
	inline std::string GetVectorAsString(sm::Vector3 v)
	{
		std::string s = std::to_string(v.x);
		s += " ";
		s += std::to_string(v.y);
		s += " ";
		s += std::to_string(v.z);
		return s;
	}
	inline std::string GetVectorAsString(sm::Vector4 v)
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

	inline void ForceRotation(sm::Vector3& v)
	{
		v.x = fmodf(v.x, d::XM_2PI);
		v.y = fmodf(v.y, d::XM_2PI);
		v.z = fmodf(v.z, d::XM_2PI);

		if (v.x < 0.f) v.x += d::XM_2PI;
		if (v.y < 0.f) v.y += d::XM_2PI;
		if (v.z < 0.f) v.z += d::XM_2PI;
		//else if (v.x > d::XM_2PI) v.x -= d::XM_2PI;
		//else if (v.y > d::XM_2PI) v.y -= d::XM_2PI;
		//else if (v.z > d::XM_2PI) v.z -= d::XM_2PI;
	}

	inline void ForceRotation(float& x, float& y, float& z)
	{
		sm::Vector3 v(x, y, z);
		ForceRotation(v);
		x = v.x;
		y = v.y;
		z = v.z;
	}




	//inline unsigned int RandomNum(int n1, int n2)
	//{
	//	if (n2 < n1)
	//	{
	//		int temp = n2;
	//		n2 = n1;
	//		n1 = temp;
	//	}
	//	
	//	return rand() % n2 + n1;;
	//}
}


