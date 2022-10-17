#pragma once
#include<DirectXMath.h>
#include <DirectXCollision.h>
#include"../Math/Simplemath.h"
#include <cmath>
#include <string>
#include <cstdint>
namespace sm = DirectX::SimpleMath;
namespace d = DirectX;

typedef UINT uint;

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