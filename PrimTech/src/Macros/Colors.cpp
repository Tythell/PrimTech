#include "pch.h"
#include "Colors.h"

namespace pt
{
	Color::Color(uchar r, uchar g, uchar b, uchar a)
	{
		m_hexClr = (uint)a;
		m_hexClr |= (uint)(b << 8);
		m_hexClr |= (uint)(g << 16);
		m_hexClr |= (uint)(r << 24);
	}

	Color::Color(uint hexColor)
	{
		m_hexClr = hexColor;
	}
	sm::Vector3 Color::GetAsVector3() const
	{
		sm::Vector3 vec;
		vec.x = (float)((m_hexClr & 0xff000000) >> 24) / 255.f;
		vec.y = (float)((m_hexClr & 0x00ff0000) >> 16) / 255.f;
		vec.z = (float)((m_hexClr & 0x0000ff00) >> 8) / 255.f;

		return vec;
	}
	sm::Vector4 Color::GetAsVector4() const
	{
		sm::Vector4 vec;
		vec.x = (float)((m_hexClr & 0xff000000)	>> 24)	/ 255.f;
		vec.y = (float)((m_hexClr & 0x00ff0000)	>> 16)	/ 255.f;
		vec.z = (float)((m_hexClr & 0x0000ff00)	>> 8)	/ 255.f;
		vec.w = (float)((m_hexClr & 0x000000ff)	>> 0)	/ 255.f;

		return vec;
	}
}

