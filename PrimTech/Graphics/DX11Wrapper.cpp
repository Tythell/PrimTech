#include "DX11Wrapper.h"
#include"../WindowWrap.h"

DX11Addon::DX11Addon(Window& window, Camera& camera) :
	m_width(window.getWinWidth()), m_height(window.getWinHeight()), m_pHWND(&window.getHWND())//,
	//m_grid(200, 90)
{
	m_pWin = &window;
	mp_cam = &camera;

	initSwapChain();
	initRTV();
	SetupDSAndVP();
	InitRastNSampState();

	InitShaders();
	InitScene();

	mp_cam->SetPosition(0, 0, -5);
	//m_fileTexture.CreateFromFile("Textures/gunter2.png", m_device);
	InitConstantBuffers();
	//m_grid.InitRenderCell(device, dc);
	//m_grid.SetCamP(*mp_cam);
	ImGuiInit(window.getHWND());
}

DX11Addon::~DX11Addon()
{
	ImGuiShutDown();

	delete im.buffer;

	device->Release();
	dc->Release();
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

	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, NULL, 0, D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &device, NULL, &dc);
	COM_ERROR(hr, "CreateDeviceAndSwapChain Failed");



	return true;
}

bool DX11Addon::initRTV()
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

	HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	COM_ERROR(hr, "Get Buffer failed");

	hr = device->CreateRenderTargetView(backBuffer.Get(), NULL, &m_rtv);
	COM_ERROR(hr, "RTV failed");

	dc->OMSetRenderTargets(1, &m_rtv, m_dsView);

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

	HRESULT hr = device->CreateTexture2D(&depthStencilTextureDesc, NULL, &m_depthStencilBuffer);
	COM_ERROR(hr, "Depth stencil failed");

	hr = device->CreateDepthStencilView(m_depthStencilBuffer, NULL, &m_dsView);
	COM_ERROR(hr, "Depth Stencil View failed");

	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	hr = device->CreateDepthStencilState(&dssDesc, &m_dsState);
	COM_ERROR(hr, "DS State failed");

	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.Width = (float)m_width;
	m_viewport.Height = (float)m_height;
	m_viewport.MinDepth = 0;
	m_viewport.MaxDepth = 0;

	dc->RSSetViewports(1, &m_viewport);

	return SUCCEEDED(hr);
}

bool DX11Addon::InitRastNSampState()
{
	D3D11_RASTERIZER_DESC rastDesc;
	ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
	rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;

	HRESULT hr = device->CreateRasterizerState(&rastDesc, &m_rasterizerState);
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

	hr = device->CreateSamplerState(&sampDesc, &m_sampState);
	COM_ERROR(hr, "Sampler State setup failed");

	return true;
}

bool DX11Addon::InitShaders()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		//{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	m_quadVs.Init(device, "../x64/Debug/QuadVS.cso");
	m_quadVs.InitInputLayout(device, layout, ARRAYSIZE(layout));
	m_quadPs.Init(device, "../x64/Debug/QuadPS.cso");

	//m_dc->VSSetShader(m_quadVs.GetShader(), NULL, 0);
	//m_dc->PSSetShader(m_quadPs.GetShader(), NULL, 0);

	D3D11_INPUT_ELEMENT_DESC layout3D[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	m_3dvs.Init(device, "../x64/Debug/vertexshader.cso");
	m_3dvs.InitInputLayout(device, layout3D, ARRAYSIZE(layout3D));
	m_3dps.Init(device, "../x64/Debug/pixelshader.cso");



	return true;
}

bool DX11Addon::InitScene()
{
	m_model.LoadObj("Assets/models/cube.txt", device, dc);
	m_missingTexture.CreateFromFile("Assets/Textures/missingtexture.png", device);

	return true;
}

void DX11Addon::InitConstantBuffers()
{
	m_transformBuffer.CreateConstantBuffer(device, dc);
	dc->VSSetConstantBuffers(0, 1, m_transformBuffer.GetReference());
}

void DX11Addon::UpdateConstantBuffers()
{

}

void DX11Addon::ImGuiInit(HWND& hwnd)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplDX11_Init(device, dc);
	ImGui_ImplWin32_Init(hwnd);
	ImGui::StyleColorsDark();
}

void DX11Addon::ImGuiRender()
{
	static bool im_appear = true;
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_NoCollapse);

	//ImGui::SetCursorPos

	ImGui::SliderFloat("Ortho view", &im.f[0], 50.f, 200.f);
	ImGui::SliderInt("Speed", &im.speed, 0, 100);

	ImGui::InputText("Text", im.buffer, 16);
	ImGui::Checkbox("Pause", &im.pause);
	//ImGui::Checkbox("Test window", &im_appear);

	if (ImGui::Button("Export image"))
		ExportImage(im.buffer);
		
	//if (mp_kb->IsKeyDown(Key::Y))
		//m_pWin->ShutDown();
	ImGui::End();
	if (mp_kb->IsKeyDown(Key::N))
	{
		ImGui::Begin("Test");
		ImGui::Text("Din mamma");
		ImGui::End();
		//OutputDebugStringA("joemama\n");
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void DX11Addon::ImGuiShutDown()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void DX11Addon::ExportImage(char* name)
{
	//m_grid.SaveImage(name);
}

void DX11Addon::SetInputP(KeyboardHandler& kb)
{
	mp_kb = &kb;
}

void DX11Addon::Render()
{
	float bgColor[] = { .1f,.1f,.1f,1.f };

	if (!im.pause)
	{
		dc->ClearRenderTargetView(m_rtv, bgColor);
		dc->ClearDepthStencilView(m_dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
		//m_dc->VSSetShader(m_quadVs.GetShader(), NULL, 0);
		//m_dc->PSSetShader(m_quadPs.GetShader(), NULL, 0);
		//m_dc->IASetInputLayout(m_quadVs.GetInputLayout());

		dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		dc->RSSetState(m_rasterizerState);
		dc->OMSetDepthStencilState(m_dsState, 0);
		dc->PSSetSamplers(0, 1, &m_sampState);
		//mp_cam->SetOrtographic(im.f[0] * (static_cast<float>(m_width) / static_cast<float>(m_height)), im.f[0], 0.f, 50.f);

		//m_grid.Update();
	}
	//UpdateConstantBuffers();

	UINT offset = 0;
	dc->VSSetShader(m_3dvs.GetShader(), NULL, 0);
	dc->PSSetShader(m_3dps.GetShader(), NULL, 0);

	dc->IASetInputLayout(m_3dvs.GetInputLayout());
	dc->VSSetConstantBuffers(0, 1, m_transformBuffer.GetReference());
	m_transformBuffer.Data().viewProj = d::XMMatrixTranspose(mp_cam->GetViewM() * mp_cam->GetProjM());
	static float rotate = 0.f;
	rotate += 0.0001f;
	m_transformBuffer.Data().world = d::XMMatrixTranspose(d::XMMatrixRotationRollPitchYawFromVector({0,rotate,0}));
	m_transformBuffer.UpdateCB();

	dc->PSSetShaderResources(0, 1, m_missingTexture.GetSRVAdress());

	m_model.Draw();

	ImGuiRender();
	m_swapChain->Present(0, NULL);
}

ID3D11Device* DX11Addon::GetDevice() const
{
	return device;
}

ID3D11DeviceContext* DX11Addon::GetDeviceContext() const
{
	return dc;
}
