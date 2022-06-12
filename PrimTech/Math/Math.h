#pragma once
#include"../Math/Simplemath.h"
namespace sm = DirectX::SimpleMath;
namespace d = DirectX;

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