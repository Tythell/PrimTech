#include"pch.h"
#include "ShadowMap.h"
#include "../Utility/Popup.h"

ShadowMap::ShadowMap(const UINT& width, const UINT& height, Camera* pcam):
	m_viewPort(0.f,0.f, width, height), m_width(width), m_height(height),
	mp_playerCam(pcam), resoulution(20.f)
{
	m_shadowCam.SetOrtographic(resoulution, resoulution, .1f, 25.f);
	//m_shadowCam.SetPerspective(80.f, 1.f, 1.f, 1000.f);
	m_shadowCam.SetPosition(0.f, 5.f, 0.f);
	m_shadowCam.SetRotation(d::XM_PIDIV2, 0.f, 0.f);
}

ShadowMap::~ShadowMap()
{
	m_depthMapDSV->Release();
	m_depthmapSRV->Release();
}

void ShadowMap::Init(ID3D11Device*& device)
{
	CD3D11_TEXTURE2D_DESC texDesc(DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS, (float)m_width, (float)m_height);
	texDesc.MipLevels = 1;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	ID3D11Texture2D* depthMap = nullptr;
	HRESULT hr = device->CreateTexture2D(&texDesc, NULL, &depthMap);
	COM_ERROR(hr, "Failed to create shadow depthmap texture");

	CD3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	COM_ERROR(device->CreateDepthStencilView(depthMap, &dsvDesc, &m_depthMapDSV), "Failed to create shadow dsv");

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	COM_ERROR(device->CreateShaderResourceView(depthMap, &srvDesc, &m_depthmapSRV), "Failed to create shadow srv");
	depthMap->Release();
}

void ShadowMap::Bind(ID3D11DeviceContext*& dc, const UINT& slot)
{
	ID3D11RenderTargetView* nulltarget[1] = { 0 };
	ID3D11ShaderResourceView* nullsrv[1] = { 0 };

	dc->RSSetViewports(1, &m_viewPort);
	dc->PSSetShaderResources(slot, 1, nullsrv);
	dc->OMSetRenderTargets(1, nulltarget, m_depthMapDSV);
	dc->ClearDepthStencilView(m_depthMapDSV, D3D11_CLEAR_DEPTH, 1.f, 0);

	if (mp_playerCam) 
	{
		m_shadowCam.SetRotation(d::XM_PIDIV2, mp_playerCam->GetRotation().y + d::XM_PIDIV4, 0.f);
		m_shadowCam.SetPosition(mp_playerCam->GetPositionXZ());
		m_shadowCam.SetOffset(8.f, -8.f, 0.f);
		//m_shadowCam.SetOffset(0.f, 0.f, -resoulution / 2);
		sm::Vector3 upVec = mp_playerCam->GetForwardVectorNoY() * ((resoulution / 2) - .6f);
		m_shadowCam.Move(0.f, 10.f, 0.f);
		//m_shadowCam.Move(-(resoulution / 4),0.f, (resoulution / 4));
	}
}

void ShadowMap::BindSRV(ID3D11DeviceContext*& dc, const UINT& slot)
{
	dc->PSSetShaderResources(slot, 1, &m_depthmapSRV);
}

void ShadowMap::SetPos(const sm::Vector3& v)
{
	m_shadowCam.SetPosition(v);
}

Camera& ShadowMap::GetShadowCam()
{
	return m_shadowCam;
}

void ShadowMap::InitModel(ID3D11DeviceContext*& dc, Buffer<hlsl::cbpWorldTransforms3D>& transformbuffer, Buffer<hlsl::cbpMaterialBuffer>& matbuffer)
{
	m_shadowCamModel.Init("bulb.obj", ModelType::eDEBUG);
	m_shadowCamModel.SetScale(1.2f);
	m_shadowCamModel.SetMaterialBuffer(matbuffer);
	m_shadowCamModel.SetDCandBuffer(dc, transformbuffer);
	m_shadowCamModel.GetMaterial().SetRimColor(MAGENTA_3F);
	m_shadowCamModel.GetMaterial().SetTransparency(.5f);
}
void ShadowMap::DrawModel()
{
	m_shadowCamModel.SetPosition(m_shadowCam.GetPosition());
	m_shadowCamModel.SetRotation(m_shadowCam.GetRotation());
	m_shadowCamModel.Rotate(d::XM_PIDIV2, 0.f, 0.f);
	m_shadowCamModel.Draw();
}

ID3D11ShaderResourceView* ShadowMap::GetSRV()
{
	return m_depthmapSRV;
}
