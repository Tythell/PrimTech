#include "pch.h"
#include "Colors.h"

namespace pt
{
	Color::Color(uchar r, uchar g, uchar b, uchar a)
	{
		//m_hexClr = (uint)a;
		//m_hexClr |= (uint)(r << 8);
		//m_hexClr |= (uint)(g << 16);
		//m_hexClr |= (uint)(b << 24);
		memcpy(&m_hexClr, &r, sizeof(uint));
	}

	Color::Color(uint hexColor)
	{
		m_hexClr = hexColor;
	}
	float3 Color::GetAsFloat3() const
	{
		return AsFloat3(m_hexClr);
	}
	float4 Color::GetAsFloat4() const
	{
		return AsFloat4(m_hexClr);;
	}
	uint Color::GetHex() const
	{
		return m_hexClr;
	}
	const float3 Color::AsFloat3(uint color)
	{
		float3 vec(0.f);
		uchar channel[3](0);

		memcpy(channel, &color, sizeof(uchar) * 3);

		vec.x = (float)channel[0] / 255.f;
		vec.y = (float)channel[1] / 255.f;
		vec.z = (float)channel[2] / 255.f;

		return vec;
	}
	const float4 Color::AsFloat4(uint color)
	{
		float4 vec(0.f);
		uchar channel[4](0);

		memcpy(channel, &color, sizeof(uchar) * 4);

		vec.x = (float)channel[0] / 255.f;
		vec.y = (float)channel[1] / 255.f;
		vec.z = (float)channel[2] / 255.f;
		vec.w = (float)channel[3] / 255.f;
		return vec;
	}
	const uint Color::AsHex(float4 color)
	{
		uchar channels[4](0);
		for (int i = 0; i < 4; i++)
			channels[i] = (uchar)(color[i] * 255);
		
		uint hex(0);
		memcpy(&hex, channels, sizeof(uint));

		return hex;
	}
}

