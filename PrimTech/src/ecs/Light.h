#pragma once
#include "Component.h"

namespace pt
{
	class TransformComp;
	class Light : public Component
	{
	public:
		Light(EntIdType entId);

		void Update(const TransformComp& transform);
		void SetColor(float4 clr) { m_lightData.clr = clr; };
		void SetOffsetPosition(float4 pos) { memcpy(&m_posOffset.x, &pos.x, sizeof(float) * 3); };
		void SetDirectionOffset(const float4& dir) { memcpy(&m_dirOffset.x, &dir.x, sizeof(float) * 3); }

		PrimtTech::hlsl::Light GetLightData() const { return m_lightData; };
		float4 GetPositionOffset() const { return m_posOffset; };
		float4 GetDirectionOffset() const { return m_dirOffset; };

		void SetType(uchar type);
		uchar GetType() const { return m_type; };
		// Inherited via Component
		virtual void DuplicateFrom(Component* other) override;
		// Inherited via Component
		virtual void OnFree() override;
	private:
		PrimtTech::hlsl::Light m_lightData;
		float4 m_posOffset;
		float4 m_dirOffset;
		uchar m_type = 0u;



		//enum Type
		//{
		//	eLightType_Point,
		//	eLightType_Directional,
		//	eLightType_Ambient,
		//};
	};
}