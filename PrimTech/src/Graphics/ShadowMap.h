#pragma once
#include"Buffer.h"
#include "Camera.h"
#include "Model.h"

namespace PrimtTech
{
	class ShadowMap
	{
	public:
		ShadowMap(const UINT& width, const UINT& height, Camera* pcam = nullptr);
		~ShadowMap();

		void Init(ID3D11Device*& device);
		void Bind(ID3D11DeviceContext*& dc, const UINT& slot);
		void BindSRV(ID3D11DeviceContext*& dc, const UINT& slot);

		void SetPos(const sm::Vector3& v);

		Camera& GetShadowCam();

		ID3D11ShaderResourceView* GetSRV();
	private:
		UINT m_width = 0, m_height = 0;
		const float resoulution;
		ID3D11ShaderResourceView* m_depthmapSRV = nullptr;
		ID3D11DepthStencilView* m_depthMapDSV = nullptr;

		CD3D11_VIEWPORT m_viewPort;
		Camera m_shadowCam;
		Camera* mp_playerCam = nullptr;
	};
}

