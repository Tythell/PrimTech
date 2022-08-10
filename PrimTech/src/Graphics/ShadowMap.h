#pragma once
#include"Buffer.h"
#include "Camera.h"
#include "Model.h"

class ShadowMap
{
public:
	ShadowMap(const UINT& width, const UINT& height);
	~ShadowMap();

	void Init(ID3D11Device*& device);
	void Bind(ID3D11DeviceContext*& dc, const UINT& slot);
	void BindSRV(ID3D11DeviceContext*& dc, const UINT& slot);

	Camera& GetShadowCam();
	void InitModel(ID3D11DeviceContext*& dc, Buffer<hlsl::cbpWorldTransforms3D>& transformbuffer, Buffer<hlsl::cbpMaterialBuffer>& matbuffer);
	void DrawModel();

	//ID3D11ShaderResourceView** GetSRV();
private:
	UINT m_width = 0, m_height = 0;

	ID3D11ShaderResourceView* m_depthmapSRV = nullptr;
	ID3D11DepthStencilView* m_depthMapDSV = nullptr;

	CD3D11_VIEWPORT m_viewPort;
	Camera m_shadowCam;

	Model m_shadowCamModel;

};