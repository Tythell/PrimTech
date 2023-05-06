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
		void SetColor(sm::Vector4 clr) { m_lightData.clr = clr; };
		void SetOffsetPosition(sm::Vector4 pos) { memcpy(&m_posOffset.x, &pos.x, sizeof(float) * 3); };
		void SetDirectionOffset(const sm::Vector4& dir) { memcpy(&m_dirOffset.x, &dir.x, sizeof(float) * 3); }

		PrimtTech::hlsl::Light GetLightData() const { return m_lightData; };
		sm::Vector4 GetPositionOffset() const { return m_posOffset; };
		sm::Vector4 GetDirectionOffset() const { return m_dirOffset; };

		void SetType(uchar type);
		uchar GetType() const { return m_type; };
		// Inherited via Component
		virtual void DuplicateFrom(Component* other) override;
		// Inherited via Component
		virtual void OnFree() override;
	private:
		PrimtTech::hlsl::Light m_lightData;
		sm::Vector4 m_posOffset;
		sm::Vector4 m_dirOffset;
		uchar m_type = 0u;



		//enum Type
		//{
		//	eLightType_Point,
		//	eLightType_Directional,
		//	eLightType_Ambient,
		//};
	};
}