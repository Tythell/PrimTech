#pragma once
#include "Math/Math.h"

namespace pt
{
	class Color
	{
	public:
		Color(uchar r, uchar g, uchar b, uchar a = 255u);
		Color(uint hexColor);

		float3 GetAsVector3() const;
		float4 GetAsVector4() const;
	private:
		uint m_hexClr;
	};
}

#define FlOATCLR3(r, g, b) r * 255, g * 255, b * 255
#define FlOATCLR4(r, g, b, a) r * 255, g * 255, b * 255, a * 255

#define BLACK_3F {0.f,0.f,0.f}
#define BLACK_4F {0.f,0.f,0.f,0.f}

#define GRAY_3F {0.5f, 0.5f, 0.5f}
#define GRAY_4F {0.4f, 0.4f, 0.4f, 0.f}

#define RED_3F {1.f,0.f,0.f}
#define RED_4F {1.f,0.f,1.f, 1.f}

#define GREEN_3F {0.f,1.f,0.f}
#define GREEN_4F {0.f,1.f,0.f,1.f}

#define BLUE_3F {0.f,0.f,1.f}
#define BLUE_4F {0.f,0.f,1.f, 1.f}

#define MAGENTA_3F {1.f,0.f,1.0f}
#define MAGENTA_4F {1.f,0.f,1.0f,1.f}

#define WHITE_3F {1.f,1.f,1.f}
#define WHITE_4F {1.f,1.f,1.f, 1.f}

#define STONE_1F 0.341f
#define STONE_3F {0.341f, 0.341f, 0.341f}

#define GOLD_3F {1.f,1.f,0.f}

#define DARKORANGE_3F {1.f, 0.549.f ,0.f}

#define ORANGE_3F {1.f, 0.672f, 0.f}

#define CYAN_3F {0.f, 1.f, 1.f}

#define LIME_3F {0.5f, 1.f, 0.f}