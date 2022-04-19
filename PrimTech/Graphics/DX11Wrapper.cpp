#include "DX11Wrapper.h"
#include"../WindowWrap.h"

DX11Addon::DX11Addon(Window& window) :
	m_width(window.getWinWidth()), m_height(window.getWinHeight()), m_pHWND(&window.getHWND())
	//m_grid(32,32)
{
	m_pWin = &window;

	initSwapChain();
	initRTV();
	SetupDSAndVP();
	InitRastNSampState();

	InitShaders();
	InitScene();
	m_cam.SetOrtographic(static_cast<float>(m_width), static_cast<float>(m_height), 0.1f, 1000.f);
	m_cam.SetPosition(0, 0, 0);
	m_defaultTexture.CreateFromFile("gunter2.png", m_device);
	InitConstantBuffers();

	m_grid.SetViewProjM(m_cam.GetProjM());
}

DX11Addon::~DX11Addon()
{
	m_device->Release();
	m_dc->Release();
	m_swapChain->Release();
	m_rtv->Release();
	m_depthStencilBuffer->Release();
	m_dsView->Release();
	m_dsState->Release();
	m_rasterizerState->Release();
	m_sampState->Release();
}

bool DX11Addon::initSwapChain()
{

	UINT flags = 0;
#ifdef _DEBUG
	flags = D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG



	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferDesc.Width = m_width;
	swapChainDesc.BufferDesc.Height = m_height;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 0; // 60
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = *m_pHWND;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, NULL, 0, D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_dc);
	COM_ERROR(hr, "CreateDeviceAndSwapChain Failed");



	return true;
}

bool DX11Addon::initRTV()
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

	HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	COM_ERROR(hr, "Get Buffer failed");

	hr = m_device->CreateRenderTargetView(backBuffer.Get(), NULL, &m_rtv);
	COM_ERROR(hr, "RTV failed");

	m_dc->OMSetRenderTargets(1, &m_rtv, m_dsView);

	return true;
}

bool DX11Addon::SetupDSAndVP()
{
	D3D11_TEXTURE2D_DESC depthStencilTextureDesc;
	depthStencilTextureDesc.Width = m_width;
	depthStencilTextureDesc.Height = m_height;
	depthStencilTextureDesc.MipLevels = 1;
	depthStencilTextureDesc.ArraySize = 1;
	depthStencilTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilTextureDesc.SampleDesc.Count = 1;
	depthStencilTextureDesc.SampleDesc.Quality = 0;
	depthStencilTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilTextureDesc.CPUAccessFlags = 0;
	depthStencilTextureDesc.MiscFlags = 0;

	HRESULT hr = m_device->CreateTexture2D(&depthStencilTextureDesc, NULL, &m_depthStencilBuffer);
	COM_ERROR(hr, "Depth stencil failed");

	hr = m_device->CreateDepthStencilView(m_depthStencilBuffer, NULL, &m_dsView);
	COM_ERROR(hr, "Depth Stencil View failed");

	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	hr = m_device->CreateDepthStencilState(&dssDesc, &m_dsState);
	COM_ERROR(hr, "DS State failed");

	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.Width = m_width;
	m_viewport.Height = m_height;
	m_viewport.MinDepth = 0;
	m_viewport.MaxDepth = 0;

	m_dc->RSSetViewports(1, &m_viewport);




	return SUCCEEDED(hr);
}

bool DX11Addon::InitRastNSampState()
{
	D3D11_RASTERIZER_DESC rastDesc;
	ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
	rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;

	HRESULT hr = m_device->CreateRasterizerState(&rastDesc, &m_rasterizerState);
	COM_ERROR(hr, "Rasterizer State setup failed");

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = m_device->CreateSamplerState(&sampDesc, &m_sampState);
	COM_ERROR(hr, "Sampler State setup failed");

	return true;
}

bool DX11Addon::InitShaders()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		//{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_APPEND_ALIGNED_ELEMENT, 0};
	};

	m_vShader.Init(m_device, "../x64/Debug/BaseVS.cso", layout, ARRAYSIZE(layout));
	m_pShader.Init(m_device, "../x64/Debug/BasePS.cso");

	m_dc->VSSetShader(m_vShader.GetShader(), NULL, 0);
	m_dc->PSSetShader(m_pShader.GetShader(), NULL, 0);

	return false;
}

bool DX11Addon::InitScene()
{
	m_grid.Init(d::XMINT2(m_width, m_height), d::XMINT2(3, 3), m_device, m_dc);
	return true;
}

void DX11Addon::InitConstantBuffers()
{
	//m_transformBuffer.Init(m_device, m_dc);
	//m_dc->VSSetConstantBuffers(0, 1, m_transformBuffer.GetReference());
}

void DX11Addon::UpdateConstantBuffers()
{
	//m_transformBuffer.getData().world = d::XMMatrixIdentity();
	//m_transformBuffer.getData().viewProj = d::XMMatrixTranspose(m_cam.GetViewM() * m_cam.GetProjM());
	//m_transformBuffer.applyChange();
}

void DX11Addon::Render()
{
	float bgColor[] = { .1,.1,.1,1 };
	m_dc->ClearRenderTargetView(m_rtv, bgColor);
	m_dc->ClearDepthStencilView(m_dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

	m_dc->IASetInputLayout(m_vShader.GetInputLayout());
	m_dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_dc->RSSetState(m_rasterizerState);
	m_dc->OMSetDepthStencilState(m_dsState, 0);
	m_dc->PSSetSamplers(0, 1, &m_sampState);
	//m_dc->PSSetShaderResources(0, 1, m_fileTexture.GetSRVAdress());


	UpdateConstantBuffers();

	m_grid.DrawCell(sm::Vector4(1,0,0, 1 ), sm::Vector2( 0,0 ));

	//m_transformBuffer.getData().world = d::XMMatrixTranspose(d::XMMatrixTranslationFromVector(sm::Vector3(-r,.4f,0)));
	//m_transformBuffer.getData().color = sm::Vector4(0, 1, 0, 1);
	//m_transformBuffer.applyChange();
	//m_dc->DrawIndexed(m_iBuffer.GetBufferSize(), 0, 0);

	m_swapChain->Present(0, NULL);
}
