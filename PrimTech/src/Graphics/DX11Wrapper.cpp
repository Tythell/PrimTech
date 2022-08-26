#include "DX11Wrapper.h"
#include"../WindowWrap.h"

#define shadowQuality 2

DX11Addon::DX11Addon(Window& window, Camera& camera) :
	m_width(window.getWinWidth()), m_height(window.getWinHeight()), m_pHWND(&window.getHWND()),
	m_shadowmap(1024 * shadowQuality, 1024 * shadowQuality), m_viewport(0.f, 0.f, (float)m_width, (float)m_height)
{
	m_pWin = &window;
	mp_cam = &camera;

	initSwapChain();
	initRTV();
	SetupDSAndVP();
	InitRastNSampState();
	InitBlendState();
	m_shadowmap.Init(device);

	ResourceHandler::SetDevice(device);


	InitShaders();
	InitConstantBuffers();
	InitScene();
	m_renderbox.Init(device, dc);
	ImGuiInit(window.getHWND());
}

DX11Addon::~DX11Addon()
{
	ImGuiShutDown();

	delete im.buffer;

	ClearModelList();

	ResourceHandler::Unload();

	device->Release();
	dc->Release();
	m_swapChain->Release();
	m_rtv->Release();
	m_depthStencilBuffer->Release();
	m_dsView->Release();
	m_dsState->Release();
	m_rasterizerState->Release();
	m_wrapSampler->Release();
	m_shadowSampler->Release();
	m_clampSampler->Release();
	m_blendState->Release();
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

	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		flags,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&m_swapChain,
		&device,
		NULL,
		&dc);
	COM_ERROR(hr, "CreateDeviceAndSwapChain Failed");

	return true;
}

bool DX11Addon::initRTV()
{
	ID3D11Texture2D* backBuffer = nullptr;

	HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	COM_ERROR(hr, "Get Buffer failed");

	hr = device->CreateRenderTargetView(backBuffer, NULL, &m_rtv);
	COM_ERROR(hr, "RTV failed");
	backBuffer->Release();

	return true;
}

void DX11Addon::SetCanMove(bool b)
{
	m_canMove = b;
}

bool DX11Addon::SetupDSAndVP()
{
	CD3D11_TEXTURE2D_DESC depthStencilTextureDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, m_width, m_height);
	depthStencilTextureDesc.MipLevels = 1;
	depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

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

	return SUCCEEDED(hr);
}

bool DX11Addon::InitRastNSampState()
{

	D3D11_RASTERIZER_DESC rastDesc;
	ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
	rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	//rastDesc.FrontCounterClockwise = false;

	HRESULT hr = device->CreateRasterizerState(&rastDesc, &m_rasterizerState);
	COM_ERROR(hr, "Rasterizer State setup failed");

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));
	//sampDesc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
	//sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	
	sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = device->CreateSamplerState(&sampDesc, &m_wrapSampler);
	COM_ERROR(hr, "Sampler State setup failed");

	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	hr = device->CreateSamplerState(&sampDesc, &m_clampSampler);
	COM_ERROR(hr, "Sampler State setup failed");

	sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;

	hr = device->CreateSamplerState(&sampDesc, &m_shadowSampler);
	COM_ERROR(hr, "Sampler State setup failed");
	//ID3D11SamplerState* samplers[] = {m_wrapSampler, m_clampSampler, m_shadowSampler};
	dc->PSSetSamplers(0, 1, &m_wrapSampler);
	dc->PSSetSamplers(1, 1, &m_clampSampler);
	dc->PSSetSamplers(2, 1, &m_shadowSampler);

	return true;
}

void DX11Addon::InitBlendState()
{
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(D3D11_RENDER_TARGET_BLEND_DESC));
	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
	rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0] = rtbd;
	COM_ERROR(device->CreateBlendState(&blendDesc, &m_blendState), "Failed to create blend state");
	//dc->OMSetBlendState(m_blendState, NULL, 0xFFFFFFFF);
}

bool DX11Addon::InitShaders()
{
	D3D11_INPUT_ELEMENT_DESC layout3D[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,	 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT",	 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",	 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	D3D11_INPUT_ELEMENT_DESC lineLayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA},
		{"COLOR",	 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA},
	};

	m_3dvs.Init(device, "vertexshader.cso");
	m_3dvs.InitInputLayout(device, layout3D, ARRAYSIZE(layout3D));
	m_3dnoLightps.Init(device, "NoLightPs.cso");
	m_toonPS.Init(device, "LightWarpPS.cso");

	m_linePS.Init(device, "QuadPS.cso");
	m_lineVS.Init(device, "QuadVS.cso");
	m_lineVS.InitInputLayout(device, lineLayout, ARRAYSIZE(lineLayout));

	dc->VSSetShader(m_3dvs.GetShader(), NULL, 0);
	//dc->PSSetShader(m_3dps.GetShader(), NULL, 0);

	return true;
}

bool DX11Addon::InitScene()
{
	dc->RSSetState(m_rasterizerState);
	dc->OMSetDepthStencilState(m_dsState, 0);
	

	dc->OMSetBlendState(m_blendState, NULL, 0xFFFFFFFF);

	//ResourceHandler::AddTexture("missingTexture.png"); // setting missingtexture
	ResourceHandler::AddTexture(".NoTexture.png"); // setting missingtexture
	ResourceHandler::AddTexture("ZANormal.png"); // Load LightWarp Texture
	//ResourceHandler::AddTexture("ZATf2esk.png"); // Load LightWarp Texture
	dc->PSSetShaderResources(0, 1, ResourceHandler::GetTexture(1).GetSRVAdress());

	m_rLine.Init(device, dc);
	m_sphere.Init(device, dc);

	//ImportScene("Scenes\\multsc.ptscene");
	NewScene();

	m_playermodel.Init("dirCapsule.obj");
	m_playermodel.SetScale(.1f);

	m_bulb.Init("bulb.obj", ModelType::eDEBUG);
	m_spotlight.Init("bulb.obj", ModelType::eDEBUG);
	m_bulb.SetScale(1.2f);
	m_bulb.GetMaterial().SetTransparency(1.f);

	m_viewmdl.Init("handmodel2.obj", mp_cam);
	m_bulb.SetMaterialBuffer(m_materialBuffer);
	m_playermodel.SetMaterialBuffer(m_materialBuffer);
	m_playermodel.SetDCandBuffer(dc, m_transformBuffer);
	m_viewmdl.m_model.SetDCandBuffer(dc, m_transformBuffer);
	m_viewmdl.m_model.SetMaterialBuffer(m_materialBuffer);
	m_bulb.SetDCandBuffer(dc, m_transformBuffer);
	m_bulb.SetMaterialBuffer(m_materialBuffer);
	m_spotlight.SetDCandBuffer(dc, m_transformBuffer);
	m_spotlight.SetMaterialBuffer(m_materialBuffer);

	dc->VSSetConstantBuffers(0, 1, m_transformBuffer.GetReference());

	for (int i = 0; i < m_models.size(); i++)
	{
		m_models[i]->SetMaterialBuffer(m_materialBuffer);
		m_models[i]->SetDCandBuffer(dc, m_transformBuffer);
	}

	m_shadowmap.InitModel(dc, m_transformBuffer, m_materialBuffer);

	//m_renderbox.Init(device);
	return true;
}

void DX11Addon::InitConstantBuffers()
{
	m_transformBuffer.CreateConstantBuffer(device, dc);
	dc->VSSetConstantBuffers(0, 1, m_transformBuffer.GetReference());
	m_lightbuffer.CreateConstantBuffer(device, dc);
	dc->PSSetConstantBuffers(0, 1, m_lightbuffer.GetReference());
	m_materialBuffer.CreateConstantBuffer(device, dc);
	dc->PSSetConstantBuffers(1, 1, m_materialBuffer.GetReference());
}

void DX11Addon::UpdateScene(const float& deltatime)
{
	m_lightbuffer.Data().ambientColor = { im.ambient[0], im.ambient[1], im.ambient[2] };
	m_lightbuffer.Data().ambientStr = im.ambient[3];
	m_lightbuffer.Data().pointLightColor = { im.pointLightColor[0], im.pointLightColor[1], im.pointLightColor[2] };
	m_lightbuffer.Data().pointlightStre = im.pointLightStr;


	m_lightbuffer.Data().direction = sm::Vector3(0.f, 1.f, 0.f);
	m_lightbuffer.Data().pointLightPosition = sm::Vector3(im.pointLightPos[0], im.pointLightPos[1], im.pointLightPos[2]);
	//m_lightbuffer.Data().forwardDir = mp_cam->GetForwardVector();
	m_lightbuffer.Data().camPos = { mp_cam->GetPosition().x, mp_cam->GetPosition().y, mp_cam->GetPosition().z, 1.f };
	m_lightbuffer.MapBuffer();


	m_playermodel.SetRotation(0.f/*-mp_cam->GetRotation().x*/, mp_cam->GetRotation().y, 0.f);
	m_playermodel.Rotate(0.f, d::XM_PI, 0.f);
	m_playermodel.SetPosition(mp_cam->GetPositionNoOffset() + sm::Vector3(0.f, -0.1f, 0.f));
	//m_model.Rotate(0.f, 2.f * deltatime, 0.f);
}

void embraceTheDarkness()
{
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg]               = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg]                = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
	colors[ImGuiCol_Border]                 = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
	colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
	colors[ImGuiCol_FrameBg]                = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
	colors[ImGuiCol_FrameBgActive]          = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_TitleBg]                = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TitleBgActive]          = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
	colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
	colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
	colors[ImGuiCol_CheckMark]              = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_SliderGrab]             = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
	colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
	colors[ImGuiCol_Button]                 = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_ButtonHovered]          = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
	colors[ImGuiCol_ButtonActive]           = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_Header]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_HeaderHovered]          = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
	colors[ImGuiCol_HeaderActive]           = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
	colors[ImGuiCol_Separator]              = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
	colors[ImGuiCol_SeparatorActive]        = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
	colors[ImGuiCol_ResizeGrip]             = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
	colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
	colors[ImGuiCol_Tab]                    = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TabHovered]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_TabActive]              = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
	colors[ImGuiCol_TabUnfocused]           = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	//colors[ImGuiCol_DockingPreview]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	//colors[ImGuiCol_DockingEmptyBg]         = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogram]          = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TableBorderLight]       = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_DragDropTarget]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowPadding                     = ImVec2(8.00f, 8.00f);
	style.FramePadding                      = ImVec2(5.00f, 2.00f);
	style.CellPadding                       = ImVec2(6.00f, 6.00f);
	style.ItemSpacing                       = ImVec2(6.00f, 6.00f);
	style.ItemInnerSpacing                  = ImVec2(6.00f, 6.00f);
	style.TouchExtraPadding                 = ImVec2(0.00f, 0.00f);
	style.IndentSpacing                     = 25;
	style.ScrollbarSize                     = 15;
	style.GrabMinSize                       = 10;
	style.WindowBorderSize                  = 1;
	style.ChildBorderSize                   = 1;
	style.PopupBorderSize                   = 1;
	style.FrameBorderSize                   = 1;
	style.TabBorderSize                     = 1;
	style.WindowRounding                    = 7;
	style.ChildRounding                     = 4;
	style.FrameRounding                     = 3;
	style.PopupRounding                     = 4;
	style.ScrollbarRounding                 = 9;
	style.GrabRounding                      = 3;
	style.LogSliderDeadzone                 = 4;
	style.TabRounding                       = 4;
}

void SetImGuiTheme()
{
	ImGui::StyleColorsDark();
	//embraceTheDarkness();

	/*ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg]               = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
	colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border]                 = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg]                = ImVec4(0.16f, 0.48f, 0.16f, 0.54f);
	colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.34f, 0.98f, 0.26f, 0.40f);
	colors[ImGuiCol_FrameBgActive]          = ImVec4(0.48f, 0.98f, 0.26f, 0.67f);
	colors[ImGuiCol_TitleBg]                = ImVec4(0.17f, 0.52f, 0.10f, 1.00f);
	colors[ImGuiCol_TitleBgActive]          = ImVec4(0.23f, 0.48f, 0.16f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark]              = ImVec4(0.39f, 0.98f, 0.26f, 1.00f);
	colors[ImGuiCol_SliderGrab]             = ImVec4(0.24f, 0.88f, 0.25f, 1.00f);
	colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.34f, 0.98f, 0.26f, 1.00f);
	colors[ImGuiCol_Button]                 = ImVec4(0.36f, 0.98f, 0.26f, 0.40f);
	colors[ImGuiCol_ButtonHovered]          = ImVec4(0.39f, 0.98f, 0.26f, 1.00f);
	colors[ImGuiCol_ButtonActive]           = ImVec4(0.06f, 0.98f, 0.23f, 1.00f);
	colors[ImGuiCol_Header]                 = ImVec4(0.26f, 0.98f, 0.28f, 0.31f);
	colors[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.98f, 0.30f, 0.80f);
	colors[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.98f, 0.35f, 1.00f);
	colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.13f, 0.75f, 0.10f, 0.78f);
	colors[ImGuiCol_SeparatorActive]        = ImVec4(0.17f, 0.75f, 0.10f, 1.00f);
	colors[ImGuiCol_ResizeGrip]             = ImVec4(0.26f, 0.98f, 0.44f, 0.20f);
	colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.98f, 0.35f, 0.67f);
	colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.98f, 0.32f, 0.95f);
	colors[ImGuiCol_Tab]                    = ImVec4(0.20f, 0.58f, 0.18f, 0.86f);
	colors[ImGuiCol_TabHovered]             = ImVec4(0.26f, 0.98f, 0.30f, 0.80f);
	colors[ImGuiCol_TabActive]              = ImVec4(0.21f, 0.68f, 0.20f, 1.00f);
	colors[ImGuiCol_TabUnfocused]           = ImVec4(0.09f, 0.15f, 0.07f, 0.97f);
	colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.17f, 0.42f, 0.14f, 1.00f);
	colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
	colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
	colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
	colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.29f, 0.98f, 0.26f, 0.35f);
	colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight]           = ImVec4(0.34f, 0.98f, 0.26f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);*/
}

void DX11Addon::ImGuiInit(HWND& hwnd)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplDX11_Init(device, dc);
	ImGui_ImplWin32_Init(hwnd);
	SetImGuiTheme();
}

void ExportToon(char* name, unsigned char* data, int offset, const int& grad1, const int& grad2)
{
	unsigned int gradSize = grad2 + (255 / 2) - grad1;
	offset += (255 / 2);
	for (int i = 0; i < 255; i++)
	{
		if (i < offset)
			data[i] = 0;
		else
			data[i] = 255;
		// if index is in gradient
		if (i < grad2 + (255 / 2) && i > grad1)
		{

		}
	}
	TextureMap::ExportCharToImage(name, data, 255, 1, 1);
}

void DX11Addon::ImGuiGradientWindow()
{
	ImGui::Begin("Gradient");
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImVec2 full_gradient_size = ImVec2(255, ImGui::GetFrameHeight());
	float halfPoint = full_gradient_size.x / 2;
	halfPoint += im.gradientOffset;
	{
		ImVec2 pbS = ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);
		ImVec2 pbE = ImVec2(pbS.x + halfPoint, full_gradient_size.y + pbS.y);
		ImVec2 pwS = ImVec2(pbS.x + halfPoint, ImGui::GetCursorScreenPos().y /* + full_gradient_size.y */);
		ImVec2 pwE = ImVec2(ImGui::GetCursorScreenPos().x + full_gradient_size.x, (full_gradient_size.y * 1) + pbS.y);
		ImU32 col_a = ImGui::GetColorU32(IM_COL32(0, 0, 0, 255));
		ImU32 col_b = ImGui::GetColorU32(IM_COL32(255, 255, 255, 255));

		ImVec2 gs = ImVec2(pbE.x - ((full_gradient_size.x / 2.f) - im.gradient[0]), pbS.y);
		//ImVec2 gs = ImVec2(pbE.x - ((full_gradient_size.x / 2.f) - im.gradient[0]), pbS.y);
		ImVec2 ge = ImVec2(pbE.x + im.gradient[1], pwE.y);

		draw_list->AddRectFilled(pbS, pbE, col_a);
		draw_list->AddRectFilled(pwS, pwE, col_b);
		draw_list->AddRectFilledMultiColor(gs, ge, col_a, col_b, col_b, col_a);
		ImGui::InvisibleButton("##gradient1", full_gradient_size);
		ImGui::SliderFloat2("grad editor", im.gradient, 1.f, 255.f / 2.f, "%.f");
		ImGui::SliderFloat("##gradient offset", &im.gradientOffset, -full_gradient_size.x / 2.f, full_gradient_size.x / 2.f, "%.f");
		ImGui::InputText("##imagename", im.buffer, 16);
		ImGui::SameLine();
		if (ImGui::Button("Export"))
		{
			//unsigned char data[255];
			//ExportToon(im.buffer, data, im.gradientOffset, im.gradient[0], im.gradient[1]);
		}
	}

	ImGui::End();
}

void LoadButton(Material* pMaterial, std::string name, TextureType e, const UINT& i = 0)
{
	bool diffExpept = (e == 0 && !pMaterial->HasTexture(eDiffuse));
	if (!diffExpept)
		name += pMaterial->GetMapName(e);

	ImGui::Text(name.c_str());
	if (diffExpept)
	{
		float diff[3]{ pMaterial->GetDiffuseClr().x, pMaterial->GetDiffuseClr().y, pMaterial->GetDiffuseClr().z};
		ImGui::SameLine();
		std::string nem = "##diffusefloat3" + std::to_string(i);
		ImGui::DragFloat3(nem.c_str(), diff, 0.01f, 0.f, 1.f);
		pMaterial->SetDiffuseClr(diff[0], diff[1], diff[2]);
	}
	ImGui::SameLine();
	std::string buttonName = "Load##" + name + std::to_string(i);
	if (ImGui::Button(buttonName.c_str()))
	{
		std::string newMtrlString = Dialogs::OpenFile("Images (*.png, *.jpg)\0*.png;*.jpg;\0", "Assets\\Textures\\");
		if (newMtrlString != "")
		{
			pMaterial->LoadTexture(newMtrlString, e);
		}
	}
	std::string remove = "Remove##";
	remove += name;
	if (pMaterial->HasTexture(e))
	{
		ImGui::SameLine();
		if (ImGui::Button(remove.c_str()))
		{
			pMaterial->RemoveTexture(e);
		}
	}

}

void DX11Addon::ImguiDebug()
{
	ImGui::Begin("Debug", &im.showDebugWindow);
	ImGui::Text("Press \"Q\" to lock/unlock mouse");

	if (ImGui::IsWindowHovered())
		m_isHoveringWindow = true;


	std::string fpsString = "FPS: " + std::to_string(m_fps);
	ImGui::Text(fpsString.c_str());

	ImGui::Checkbox("Show selection", &im.showSelection);
	if (ImGui::CollapsingHeader("ShadowMap"))
	{
		ImGui::Checkbox("Shadows", &im.shadowMap); ImGui::SameLine();
		ImGui::Checkbox("View Shadowcam", &im.viewshadowcam);
		ImGui::DragFloat3("Pos", im.shadowcamPos, 0.1f);
		ImGui::DragFloat3("Rotate", im.shadowcamrotation, 0.1f);
		m_shadowmap.GetShadowCam().SetRotation(im.shadowcamrotation[0], im.shadowcamrotation[1], im.shadowcamrotation[2]);
		m_shadowmap.GetShadowCam().SetPosition(im.shadowcamPos[0], im.shadowcamPos[1], im.shadowcamPos[2]);
		if (ImGui::Button("TP cam"))
		{
			im.shadowcamrotation[0] = mp_cam->GetRotation().x;
			im.shadowcamrotation[1] = mp_cam->GetRotation().y;
			im.shadowcamrotation[2] = mp_cam->GetRotation().z;
			im.shadowcamPos[0] = mp_cam->GetPosition().x;
			im.shadowcamPos[1] = mp_cam->GetPosition().y;
			im.shadowcamPos[2] = mp_cam->GetPosition().z;
		}
		if (ImGui::Button("winname"))
		{
			::SetWindowTextA(*m_pHWND, "balls");
		}
	}
	if (ImGui::CollapsingHeader("General"))
	{
		ImGui::RadioButton("local", (int*)&im.transformMode, 0); ImGui::SameLine();
		ImGui::RadioButton("world", (int*)&im.transformMode, 1);
		if (ImGui::SliderInt("FOV", &im.fov, 40, 110))
			mp_cam->SetPerspective((float)im.fov, (float)m_width / (float)m_height, .1f, 100.f);
		ImGui::Checkbox("Vsync", &im.useVsync); ImGui::SameLine();
		ImGui::Checkbox("Handmodel", &im.enableHandModel); ImGui::SameLine();
		ImGui::Checkbox("Raycast", &im.drawRayCast);
		ImGui::SameLine();
		ImGui::Checkbox("BCircle", &im.drawBCircle);
	}

	if (ImGui::CollapsingHeader("Light"/*, ImGuiTreeNodeFlags_DefaultOpen */))
	{
		ImGui::DragFloat4("Ambient", im.ambient, 0.002f, 0.f, 1.f);
		ImGui::DragFloat3("Pointlight Position", im.pointLightPos, 0.01f);
		ImGui::DragFloat3("PointLightColor", im.pointLightColor, 0.01f, 0.f, 1.f);
		m_bulb.GetMaterial().SetRimColor({ im.pointLightColor[0], im.pointLightColor[1], im.pointLightColor[2] });
		ImGui::SliderFloat("PointLight Strength", &im.pointLightStr, 0.f, 3.f);
		ImGui::SliderFloat("Specular power", &im.specPow, 1.f, 1000.f);
		ImGui::SliderFloat("Distance", &im.pointLightDistance, 0.f, 100.f);

		std::string shadowCamString = "Shadow cam dir: " + GetVectorAsString(m_shadowmap.GetShadowCam().GetForwardVector());
		ImGui::Text(shadowCamString.c_str());

		ImGui::SliderFloat("Attenuation", &im.atten, 0.f, 10.f);
	}

	if (ImGui::CollapsingHeader("SpotLight"/*, ImGuiTreeNodeFlags_DefaultOpen */))
	{
		ImGui::DragFloat3("Position##sl", im.sl.position, 0.01f);
		if (ImGui::Button("Move to cam##sl"))
		{
			//m_spotlight.SetPosition(mp_cam->GetPosition());
			//m_spotlight.SetRotation(mp_cam->GetForwardVector());
			im.sl.position[0] = mp_cam->GetPosition().x;
			im.sl.position[1] = mp_cam->GetPosition().y;
			im.sl.position[2] = mp_cam->GetPosition().z;

			im.sl.rotation[0] = mp_cam->GetRotation().x * d::XM_PIDIV2;
			im.sl.rotation[1] = mp_cam->GetRotation().y * d::XM_PIDIV2;
			im.sl.rotation[2] = mp_cam->GetRotation().z * d::XM_PIDIV2;
		}
		//ImGui::DragFloat3("Angle##sl", im.sl.rotation, 0.01f);
		//ImGui::DragFloat3("Color##sl", im.pointLightColor, 0.01f, 0.f, 1.f);
		//m_bulb.GetMaterial().SetRimColor({ im.pointLightColor[0], im.pointLightColor[1], im.pointLightColor[2] });
		ImGui::SliderFloat("Strength##sl", &im.pointLightStr, 0.f, 3.f);
		ImGui::SliderFloat("Specular power##sl", &im.specPow, 1.f, 1000.f);
		//ImGui::SliderFloat("Distance", &im.pointLightDistance, 0.f, 100.f);

		std::string shadowCamString = "Shadow cam dir: " + GetVectorAsString(m_shadowmap.GetShadowCam().GetForwardVector());
		ImGui::Text(shadowCamString.c_str());

		ImGui::SliderFloat("Attenuation", &im.atten, 0.f, 10.f);
	}

	m_lightbuffer.Data().pointLightDistance = im.pointLightDistance;
	m_lightbuffer.Data().specularInstensity = im.specPow;
	m_lightbuffer.Data().shadowDir = m_shadowmap.GetShadowCam().GetForwardVector();
	m_lightbuffer.Data().atten = im.atten;

	std::string camoffsetString = "Camera offset: ";
	camoffsetString += std::to_string(mp_cam->GetOffset().z);
	ImGui::Text(camoffsetString.c_str());

	m_bulb.SetPosition(im.pointLightPos[0], im.pointLightPos[1], im.pointLightPos[2]);

	ImGui::Text(GetVectorAsString(mp_cam->GetRotation()).c_str());
	ImGui::End();
}

void DX11Addon::ImGuiRender()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
	m_isHoveringWindow = false;
	ImGuizmo();

	ImGuiMenu();
	if (im.showDebugWindow) ImguiDebug();
	if (im.showShadowMapDepth) ImGuTextureDisplay();

	//ImGuiGradientWindow();
	ImGuiEntList();


	if (im.showDemoWindow)
		ImGui::ShowDemoWindow(&im.showDemoWindow);

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

//#define PRINTER(name) printer(#name, )

void DX11Addon::ImGuiShutDown()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void DX11Addon::ImGuiMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::IsWindowHovered())
			m_isHoveringWindow = true;
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::IsWindowHovered())
				m_isHoveringWindow = true;
			if (ImGui::MenuItem("New Scene", NULL, false, true))
			{
				ResourceHandler::ResetUses();
				NewScene();
			}
			if (ImGui::MenuItem("Open Scene...", NULL, false, true))
			{
				std::string diapath = Dialogs::OpenFile("Scene (*.ptscene)\0*.ptscene\0", "Scenes\\");
				if (diapath != "")
				{
					ClearModelList();
					ResourceHandler::ResetUses();
					ImportScene(diapath);
					for (int i = 0; i < m_models.size(); i++)
					{
						m_models[i]->SetDCandBuffer(dc, m_transformBuffer);
						m_models[i]->SetMaterialBuffer(m_materialBuffer);
					}
				}
			}
			if (ImGui::MenuItem("Save scene as..."))
			{
				std::string diapath = Dialogs::SaveFile("Scene (*.ptscene)\0*.ptscene\0", "Scenes\\");
				if (diapath != "")
				{
					if (StringHelper::GetExtension(diapath) != "ptscene")
						diapath += ".ptscene";
					ExportScene(diapath);
				}
			}
			ImGui::BeginDisabled(true);
			if (ImGui::MenuItem("Update Materials..."))
			{
				std::vector<std::string> diapath = Dialogs::OpenMultifile("Pmaterial (*.pmtrl)\0*.pmtrl\0", "Assets\\pmtrl");
				if (!diapath.empty())
				{
					for (int i = 1; i < diapath.size(); i++)
					{
						Material mat;
						mat.ImportMaterial(diapath[i]);
						mat.ExportMaterial("Assets/pmtrl/newmats/" + diapath[i]);
					}
				}
			}
			ImGui::EndDisabled();

			if (ImGui::MenuItem("Exit PrimTech"))
			{
				ShutDown();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Lighting"))
		{
			if (ImGui::MenuItem("Load Lightwarp"))
			{
				std::string path = Dialogs::OpenFile("Images (*.png)\0*.png;*.jpg", "Assets\\Textures\\");
				if (!path.empty()) SetLightWarp(path);
			}
			ImTextureID lightwarptex = ResourceHandler::GetTextureAdress(1)->GetSRV();
			ImGui::Separator();
			ImGui::Image(lightwarptex, { 255.f, 10.f });

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Windows"))
		{
			ImGui::MenuItem("Debug", NULL, &im.showDebugWindow);

			ImGui::MenuItem("ImGui Demo", NULL, &im.showDemoWindow);
			ImGui::Separator();
			ImGui::MenuItem("ShadowMapDepth", NULL, &im.showShadowMapDepth);

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Utility"))
		{
			if (ImGui::MenuItem("TP to spawn", NULL))
				mp_cam->SetPosition(0.f, 1.f, 0.f);

			ImGui::EndMenu();
		}


		ImGui::EndMainMenuBar();
	}
}

int NewModelToV(std::string path, ID3D11DeviceContext*& dc, Buffer<hlsl::cbpWorldTransforms3D>& tbuffer, Buffer<hlsl::cbpMaterialBuffer>& mbuffer,
	ModelList& modelV)
{
	path = StringHelper::GetName(path);
	modelV.emplace_back(new Model);
	modelV[modelV.size() - 1]->Init(path);
	modelV[modelV.size() - 1]->SetDCandBuffer(dc, tbuffer);
	modelV[modelV.size() - 1]->SetMaterialBuffer(mbuffer);

	return ((int)modelV.size() - 1);
}

int InterfaceAddModelToVector(ID3D11DeviceContext*& dc, Buffer<hlsl::cbpWorldTransforms3D>& tbuffer, Buffer<hlsl::cbpMaterialBuffer>& mbuffer,
	ModelList& modelV)
{
	std::string path = Dialogs::OpenFile("Model (*.obj)\0*.obj;*.txt\0", "Assets\\models\\");
	if (!path.empty())
	{
		path = StringHelper::GetName(path);
		return NewModelToV(path, dc, tbuffer, mbuffer, modelV);
	}
	return -1;
}



int CopyModel(ID3D11DeviceContext*& dc, Buffer<hlsl::cbpWorldTransforms3D>& tbuffer, Buffer<hlsl::cbpMaterialBuffer>& mbuffer,
	ModelList& modelV, const UINT& i)
{
	std::string name = modelV[i]->GetName();
	int brackedIndex = -1;
	for (int i = 0; i < name.size() && brackedIndex == -1; i++)
	{
		if (name[i] == ')')
			brackedIndex = i - 1;
	}
	if (brackedIndex != -1)
		name = name.substr(brackedIndex + 2);
	modelV.emplace_back(new Model);
	UINT newIndex = (UINT)modelV.size() - 1;
	modelV[newIndex]->Init(name);
	modelV[newIndex]->SetDCandBuffer(dc, tbuffer);
	modelV[newIndex]->SetMaterialBuffer(mbuffer);
	modelV[newIndex]->SetScale(modelV[i]->GetScale());
	modelV[newIndex]->SetRotation(modelV[i]->GetRotation());
	modelV[newIndex]->SetPosition(modelV[i]->GetPosition());

	return (int)newIndex;
}

void DX11Addon::ImGuiEntList()
{
	ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Model List##2", (bool*)false/*, /*ImGuiWindowFlags_MenuBar*/))
	{
		if (ImGui::IsWindowHovered())
			m_isHoveringWindow = true;
		if (ImGui::Button(" + ##Button"))
			ImGui::OpenPopup("popup##context");
		if (ImGui::BeginPopup("popup##context"))
		{
			if (ImGui::Selectable("Model"))
				m_selected = InterfaceAddModelToVector(dc, m_transformBuffer, m_materialBuffer, m_models);

			ImGui::EndPopup();
		}

		ImGui::BeginChild("Lefty", ImVec2(150, 350), true);
		if (ImGui::IsWindowHovered())
			m_isHoveringWindow = true;
		for (int i = 0; i < m_models.size(); i++)
		{
			if (ImGui::Selectable(m_models[i]->GetName().c_str(), m_selected == i))
				m_selected = i;
		}

		ImGui::EndChild();
		ImGui::End();

		if (m_selected != -1)
		{
			ImGui::Begin("Model properties");
			Model* pSelectedModel = m_models[m_selected];

			if (ImGui::IsWindowHovered())
				m_isHoveringWindow = true;

			ImGui::BeginGroup();
			ImGui::Text(m_models[m_selected]->GetName().c_str());
			ImGui::Separator();
			if (ImGui::CollapsingHeader("Transformations", ImGuiTreeNodeFlags_DefaultOpen))
			{
				sm::Vector3 pos = pSelectedModel->GetPosition();
				float position[3] = { pos.x, pos.y, pos.z };
				ImGui::PushItemWidth(m_width * .105f);
				ImGui::DragFloat3("Position", position, .02f);
				ImGui::SameLine();
				if (ImGui::Button("Reset##Position")) for (int i = 0; i < 3; i++)
					position[i] = 0.f;
				pSelectedModel->SetPosition(position[0], position[1], position[2]);

				sm::Vector3 rot = pSelectedModel->GetRotation();
				float rotation[3] = { rot.x, rot.y, rot.z };
				ImGui::DragFloat3("Rotation", rotation, .02f);

				ImGui::SameLine();
				if (ImGui::Button("Reset##Rotation"))
					for (int i = 0; i < 3; i++)
						rotation[i] = 0.f;
				pSelectedModel->SetRotation(rotation[0], rotation[1], rotation[2]);

				sm::Vector3 scale = pSelectedModel->GetScale();
				float scalef[3] = { scale.x, scale.y, scale.z };

				ImGui::DragFloat3("Scale   ", scalef, .02f);


				ImGui::SameLine();
				if (ImGui::Button("Reset##Scale"))
					for (int i = 0; i < 3; i++)
						scalef[i] = 1.f;

				ImGui::SameLine();
				if (ImGui::Button("X"))
				{
					scalef[1] = scalef[0];
					scalef[2] = scalef[0];
				}
				pSelectedModel->SetScale(scalef[0], scalef[1], scalef[2]);
			}
			for (UINT matIndex = 0; matIndex < pSelectedModel->GetMeshP()->GetNofMeshes(); matIndex++)
			{
				Material* pMaterial = &pSelectedModel->GetMaterial(matIndex);
				std::string matName = "Material - " + std::to_string(matIndex);
				if (ImGui::CollapsingHeader(matName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
				{
					LoadButton(pMaterial, "Diffuse: ", eDiffuse, matIndex);
					LoadButton(pMaterial, "NormalMap: ", eNormal, matIndex);
					LoadButton(pMaterial, "DistMap: ", eDistortion, matIndex);
					LoadButton(pMaterial, "OpacityMap: ", eOpacity, matIndex);

					sm::Vector2 diffuseSpeed(pMaterial->GetDiffuseScrollSpeed());
					sm::Vector2 distortionSpeed(pMaterial->GetDistortionScrollSpeed());
					bool hasDistMap = pMaterial->HasTexture(eDistortion);
					bool hasDiffuse = pMaterial->HasTexture(eDiffuse) || pMaterial->HasTexture(eNormal) || pMaterial->HasTexture(eOpacity);

					float diffSpeed[2]{ diffuseSpeed.x, diffuseSpeed.y };
					float distSpeed[2]{ distortionSpeed.x, distortionSpeed.y };

					std::string title = "Diffuse Scroll" + std::to_string(matIndex);
					ImGui::PushItemWidth(m_width * 0.14f);
					if (hasDiffuse)
					{
						ImGui::SliderFloat2(title.c_str(), diffSpeed, -.5f, .5f);
					}
					

					title = "Distortion Scroll##" + std::to_string(matIndex);
					if (hasDistMap)
						ImGui::SliderFloat2(title.c_str(), distSpeed, -.5f, .5f);

					title = "Reset Scrollspeed##" + std::to_string(matIndex);
					if (ImGui::Button(title.c_str()))
					{
						for (int i = 0; i < 2; i++)
						{
							diffSpeed[i] = 0.f;
							distSpeed[i] = 0.f;
						}
					}
					ImGui::SameLine();
					title = "Reset scrollvalues##" + std::to_string(matIndex);
					if (ImGui::Button(title.c_str()))
						pMaterial->ResetScrollValue();

					pMaterial->SetDiffuseScrollSpeed(diffSpeed[0], diffSpeed[1]);
					pMaterial->SetDistortionScrollSpeed(distSpeed[0], distSpeed[1]);

					if (hasDistMap)
					{
						int distDivider = pMaterial->GetDistortionDivider();
						title = "Dist divider##" + std::to_string(matIndex);
						ImGui::SliderInt(title.c_str(), &distDivider, 1, 20);
						pMaterial->SetDistortionDivider(distDivider);
					}

					float transparancy = pMaterial->GetTransparancy();
					title = "Transparancy##" + std::to_string(matIndex);
					ImGui::SliderFloat(title.c_str(), &transparancy, 0.f, 1.f);
					pMaterial->SetTransparency(transparancy);

					float tiling = pMaterial->GetTextureScale();
					title = "Tiling##" + std::to_string(matIndex);
					ImGui::SliderFloat(title.c_str(), &tiling, 1.f, 10.f);
					pMaterial->SetTextureScale(tiling);
					if (hasDistMap)
					{
						float distScale = pMaterial->GetTextureScaleDist();

						title = "Dist Scale##" + std::to_string(matIndex);
						ImGui::SliderFloat(title.c_str(), &distScale, 1.f, 10.f);
						pMaterial->SetTextureScaleDist(distScale);
					}

					title = "Import##" + std::to_string(matIndex);
					if (ImGui::Button(title.c_str()))
					{
						std::string savePath = "";
						savePath = Dialogs::OpenFile("Material (*.pmtrl)\0*.pmtrl;\0", "Assets\\pmtrl\\");

						if (savePath != "")
						{
							std::string test = StringHelper::GetExtension(savePath);
							if (!(test != "pmtrl" || test != "mtl"))
								savePath += ".pmtrl";
							pMaterial->ImportMaterial(StringHelper::GetName(savePath));
						}
					}
					ImGui::SameLine();
					title = "Export##" + std::to_string(matIndex);
					if (ImGui::Button(title.c_str()))
					{
						std::string savePath = "";
						savePath = Dialogs::SaveFile("Material (*.pmtrl)\0*.pmtrl\0", "Assets\\pmtrl\\");

						if (savePath != "")
						{
							std::string test = StringHelper::GetExtension(savePath);
							if (test != "pmtrl")
								savePath += ".pmtrl";
							pMaterial->ExportMaterial(savePath);
						}
					}

				}
			}
			if (ImGui::CollapsingHeader("Character Light", ImGuiTreeNodeFlags_DefaultOpen))
			{
				float light[]{ pSelectedModel->GetCharacterLight(0).x,
					pSelectedModel->GetCharacterLight(0).y,
					pSelectedModel->GetCharacterLight(0).z,
					pSelectedModel->GetCharacterLight(0).w };

				ImGui::DragFloat4("DirLight", light, 0.01f);

				pSelectedModel->SetLight(sm::Vector4(light[0], light[1], light[2], light[3]), 0);
			}
			ImGui::EndGroup();
			if (mp_kb->IsKeyDown(Key::DELETEKEY))
			{
				m_models[m_selected]->DecreaseMeshUsage();
				delete m_models[m_selected];
				m_models.erase(m_models.begin() + m_selected);
				m_selected = -1;
			}
			static bool ispressed = false;
			if (mp_kb->IsKeyDown(Key::CONTROL) && mp_kb->IsKeyDown(Key::D) && !ispressed)
			{
				CopyModel(dc, m_transformBuffer, m_materialBuffer, m_models, m_selected);
				ispressed = true;
			}
			else if (!mp_kb->IsKeyDown(Key::CONTROL) && !mp_kb->IsKeyDown(Key::D))
				ispressed = false;

			ImGui::End();
		}
		for (int i = 0; i < m_models.size(); i++)
			for (UINT m = 0; m < m_models[i]->GetMeshP()->GetNofMeshes(); m++)
			{
				m_models[i]->GetMaterial(m).SetSelection(false);
			}
		if (m_selected != -1 && im.showSelection)
			for (UINT m = 0; m < m_models[m_selected]->GetMeshP()->GetNofMeshes(); m++)
				m_models[m_selected]->GetMaterial(m).SetSelection(true);
	}
}

void DX11Addon::ImportScene(std::string path)
{
	m_selected = -1;
	std::ifstream reader(path, std::ios::binary | std::ios::in);
	if (!reader.is_open())
		Popup::Error("Failed open scene");
	Sceneheaders header = Sceneheaders::enull;
	RecursiveRead(header, m_models, reader);
	reader.close();
}

int countDigits(int n)
{
	int total = 0;

	while (n > 0) {
		n = n / 10;
		total++;
	}
	return total;
}

void DX11Addon::ExportScene(std::string path)
{
	std::ofstream writer(path, std::ios::binary | std::ios::out);

	Sceneheaders header = Sceneheaders::eMODEL;
	for (int i = 0; i < m_models.size(); i++)
	{
		ModelStruct ms;
		strcpy_s(ms.modelname, m_models[i]->GetName().c_str());
		if (ms.modelname[0] == '(')
		{
			std::string strCopy(ms.modelname);
			int brackedIndex = -1;
			for (int i = 0; i < strCopy.size() && brackedIndex == -1; i++)
			{
				if (strCopy[i] == ')')
					brackedIndex = i - 1;
			}
			if (brackedIndex != -1)
				strCopy = strCopy.substr(brackedIndex + 2);
			sprintf_s(ms.modelname, strCopy.c_str());
		}
		ms.scale = m_models[i]->GetScale();
		ms.rotation = m_models[i]->GetRotation();
		ms.position = m_models[i]->GetPosition();
		ms.noOfExtraMats = m_models[i]->GetMeshP()->GetNofMeshes() - 1;
		strcpy_s(ms.mtrlname, m_models[i]->GetMaterial().GetFileName().c_str());
		writer.write((const char*)&header, 4);
		writer.write((const char*)&ms, sizeof(ModelStruct));
		for (UINT eI = 0; eI < ms.noOfExtraMats; eI++)
		{
			strcpy_s(ms.mtrlname, m_models[i]->GetMaterial(eI + 1).GetFileName().c_str());
			writer.write(ms.mtrlname, 24);
		}
	}
	header = Sceneheaders::enull;
	writer.write((const char*)&header, 4);
	writer.close();
}

void DX11Addon::ImGuizmo()
{
	static ImGuizmo::OPERATION op = ImGuizmo::OPERATION::TRANSLATE;
	if (!m_canMove)
	{
		if (mp_kb->IsKeyDown(Key::W))
			op = ImGuizmo::OPERATION::TRANSLATE;
		else if (mp_kb->IsKeyDown(Key::E))
			op = ImGuizmo::OPERATION::ROTATE;
		else if (mp_kb->IsKeyDown(Key::R))
			op = ImGuizmo::OPERATION::SCALE;
	}

	if (m_selected != -1 && !m_canMove)
	{
		if (ImGuizmo::IsOver() || ImGuizmo::IsUsing())
			m_isHoveringWindow = true;
		ImGui::SetNextWindowSize(ImVec2((float)m_width, (float)m_height));
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs;
		ImGui::Begin("##GizmoWin", 0, flags);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, (float)m_width, (float)m_height);

		sm::Matrix camViewM = mp_cam->GetViewM();
		sm::Matrix camProj = mp_cam->GetProjM();
		sm::Matrix world = m_models[m_selected]->GetWorld();


		float* model = reinterpret_cast<float*>(&world);
		const float* proj = reinterpret_cast<const float*>(&camProj);
		const float* view = reinterpret_cast<const float*>(&camViewM);

		ImGuizmo::Manipulate(view, proj, op, im.transformMode, model);

		if (ImGuizmo::IsUsing())
		{
			sm::Vector3 pos;
			sm::Vector3 scale;
			sm::Quaternion rot;
			world.Decompose(scale, rot, pos);

			m_models[m_selected]->SetScale(scale);
			m_models[m_selected]->SetRotation(rot);
			m_models[m_selected]->SetPosition(pos);
		}

		ImGui::End();
	}
}

void DX11Addon::ImGuTextureDisplay()
{
	float winvar = 400;
	float offset = 35;
	ImVec2 winSize(winvar, winvar + offset);
	ImGui::SetNextWindowSize(winSize);

	ImGui::Begin("Texture Display", &im.showShadowMapDepth, ImGuiWindowFlags_NoResize);

	ImTextureID tex = m_shadowmap.GetSRV();
	ImGui::Image(tex, { winSize.x, winSize.x });
	if (ImGui::IsWindowHovered())
		m_isHoveringWindow = true;

	ImGui::End();
}

void DX11Addon::NewScene()
{
	m_selected = -1;
	ClearModelList();
	m_selected = -1;
	m_models.emplace_back(new Model);
	UINT i = (UINT)m_models.size() - 1;
	m_models[i]->Init("scaledplane.obj");
	m_models[i]->SetDCandBuffer(dc, m_transformBuffer);
	m_models[i]->SetMaterialBuffer(m_materialBuffer);
	m_models[i]->Scale(10.f);
	m_models[i]->SetPosition(0.f, -.5f, 0.f);
	//m_models[i]->LoadTexture("tfground.png");
}

void DX11Addon::ClearModelList()
{
	for (int i = 0; i < m_models.size(); i++)
	{
		delete m_models[i];
	}
	m_models.clear();
}

void DX11Addon::SetLightWarp(const std::string& path)
{
	//int textureIndex = ResourceHandler::CheckTextureNameExists(StringHelper::GetName(path));
	//if (textureIndex != -1)
	//	dc->PSSetShaderResources(0, 1, ResourceHandler::GetTexture(textureIndex).GetSRVAdress());
	//else
	//{
	//	TextureMap* tex = ResourceHandler::AddTexture(path);
	//	dc->PSSetShaderResources(0, 1, tex->GetSRVAdress());
	//}
	ResourceHandler::GetTextureAdress(1)->CreateFromFile(path.c_str(), device);
	dc->PSSetShaderResources(0, 1, ResourceHandler::GetTexture(1).GetSRVAdress());
}

void DX11Addon::CalculateFps(const float& deltatime)
{
	static float fpsTimer = 0.f;
	static int fpsCounter = 0;
	fpsTimer += deltatime;
	fpsCounter++;
	if (fpsTimer >= 1.f)
	{
		m_fps = fpsCounter;
		fpsCounter = 0;
		fpsTimer = 0.f;
	}
}

float GetHighestValue(const sm::Vector3& v)
{
	float arr[3] = { v.x, v.y, v.z };
	float value = -999999;
	for (int i = 0; i < 3; i++)
	{
		if (arr[i] > value) value = arr[i];
	}
	return value;
}

float GetAvarageValue(const sm::Vector3& v)
{
	float value = (v.x + v.y + v.z) / 3;

	return value;
}

int ClickFoo(const sm::Ray& ray, ModelList& models)
{
	float maxDistance = 100.f;
	float t = 0.f;
	int index = -1;

	d::BoundingSphere transformedSphere;
	for (int i = 0; i < models.size(); i++)
	{
		transformedSphere = models[i]->GetBSphere();
		sm::Vector3 center = transformedSphere.Center;

		float radius = transformedSphere.Radius;

		radius *= GetHighestValue(models[i]->GetScale());

		sm::Vector3 transformedCenter = d::XMVector3TransformCoord(center, models[i]->GetWorld());

		transformedSphere.Center = transformedCenter;
		transformedSphere.Radius = radius;

		float dummy = 0.f;
		if (ray.Intersects(transformedSphere, dummy))
		{
			sm::Ray localSpaceRay;
			sm::Matrix invWorld = models[i]->GetWorldInversed();
			localSpaceRay.position = d::XMVector3TransformCoord(ray.position, invWorld);
			localSpaceRay.direction = d::XMVector3TransformNormal(ray.direction, invWorld);
			localSpaceRay.direction.Normalize();
			for (UINT k = 0; k < models[i]->GetMeshP()->GetNofMeshes(); k++)
			{
				Buffer<Vertex3D>* pVbuffer = &models[i]->GetMeshP()->GetVBuffer();
				UINT nOTriangles = pVbuffer->GetBufferSize() / 3;
				for (UINT j = 0; j < nOTriangles; j++)
				{
					sm::Vector3 tri0 = pVbuffer->Data((j * 3) + 0).position;
					sm::Vector3 tri1 = pVbuffer->Data((j * 3) + 1).position;
					sm::Vector3 tri2 = pVbuffer->Data((j * 3) + 2).position;
					if (localSpaceRay.Intersects(tri0, tri1, tri2, t))
					{
						// Multiply distnace to world space
						t *= GetAvarageValue(models[i]->GetScale());
						if (t < maxDistance)
						{
							maxDistance = t;
							index = i;
						}
					}
				}
			}

			//if (t > maxDistance)
			//{
			//	maxDistance = t;
			//	index = i;
			//}

		}
	}
	return index;
}

void DX11Addon::Click(const sm::Vector3& dir)
{
	if (!m_isHoveringWindow)
	{
		sm::Ray ray;
		ray.position = mp_cam->GetPosition();
		ray.direction = dir;
		//#ifdef _DEBUG
		if (im.drawRayCast)
		{
			sm::Vector3 endPos = ray.position + dir;
			m_rLine.SetLine(ray.position, endPos);
		}
		//#endif // _DEBUG
		int n = ClickFoo(ray, m_models);
		m_selected = n;
	}
}

void DX11Addon::SetInputP(KeyboardHandler& kb)
{
	mp_kb = &kb;
}

void DX11Addon::Render(const float& deltatime)
{
	int modelAmount = (int)m_models.size();
	float bgColor[] = { .1f,.1f,.1f,1.f };

	dc->ClearRenderTargetView(m_rtv, bgColor);
	dc->ClearDepthStencilView(m_dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	dc->OMSetBlendState(m_blendState, NULL, 0xFFFFFFFF);

	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	if (im.shadowMap)
	{
		dc->IASetInputLayout(m_3dvs.GetInputLayout());
		dc->VSSetShader(m_3dvs.GetShader(), NULL, 0);
		dc->PSSetShader(NULL, NULL, 0);
		m_transformBuffer.Data().viewProj = d::XMMatrixTranspose(m_shadowmap.GetShadowCam().GetViewM() * m_shadowmap.GetShadowCam().GetProjM());
		m_transformBuffer.Data().lightViewProj = d::XMMatrixTranspose(m_shadowmap.GetShadowCam().GetViewM() * m_shadowmap.GetShadowCam().GetProjM());
		m_shadowmap.Bind(dc, 10);

		m_playermodel.Draw();
		for (int i = 0; i < modelAmount; i++)
			m_models[i]->Draw();
		//dc->OMSetRenderTargets(1, NULL, NULL);
	}

	dc->OMSetRenderTargets(1, &m_rtv, m_dsView);
	dc->RSSetViewports(1, &m_viewport);

	dc->IASetInputLayout(m_lineVS.GetInputLayout());
	dc->VSSetShader(m_lineVS.GetShader(), NULL, 0);
	dc->PSSetShader(m_linePS.GetShader(), NULL, 0);

	if (!im.viewshadowcam)
		m_transformBuffer.Data().viewProj = d::XMMatrixTranspose(mp_cam->GetViewM() * mp_cam->GetProjM());

	if (im.drawBCircle && m_selected != -1)
	{
		dc->VSSetShader(m_lineVS.GetShader(), NULL, 0);
		dc->PSSetShader(m_linePS.GetShader(), NULL, 0);
		dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
		d::BoundingSphere sphere;
		sphere = m_models[m_selected]->GetBSphere();
		sm::Vector3 center = sphere.Center;
		//center = d::XMVector3TransformCoord(center, d::XMMatrixRotationRollPitchYawFromVector(m_models[m_selected].GetRotation()));

		sm::Matrix rotMat = d::XMMatrixRotationRollPitchYawFromVector(m_models[m_selected]->GetRotation());
		sm::Matrix scaleMat = d::XMMatrixScalingFromVector(m_models[m_selected]->GetScale());
		sm::Vector3 transformedCenter = d::XMVector3TransformCoord(center, scaleMat * rotMat);

		//sm::Vector3 scalar = d::XMVector3TransformCoord(center, d::XMMatrixScalingFromVector(m_models[m_selected].GetScale()));
		center = transformedCenter;
		//center *= d::XMVector3TransformCoord(center, d::XMMatrixRotationRollPitchYawFromVector(m_models[m_selected].GetRotation()));
		sphere.Center = center;
		float radius = sphere.Radius * GetHighestValue(m_models[m_selected]->GetScale());
		radius *= 2.0f;
		//if (extents.y == 0.f) extents.y = 0.01f;
		sm::Vector3 position = m_models[m_selected]->GetPosition() + sphere.Center;
		sm::Matrix boxMatrix = d::XMMatrixTranspose(d::XMMatrixScaling(radius, radius, radius) * d::XMMatrixTranslationFromVector(position));
		m_transformBuffer.Data().world = boxMatrix;
		m_transformBuffer.MapBuffer();
		m_sphere.Draw(dc);
	}
	if (im.drawRayCast)
	{
		dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
		m_transformBuffer.Data().world = d::XMMatrixIdentity();
		m_transformBuffer.MapBuffer();
		m_rLine.Draw(dc);
	}

	dc->IASetInputLayout(m_3dvs.GetInputLayout());
	dc->VSSetShader(m_3dvs.GetShader(), NULL, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//dc->PSSetShader(m_3dps.GetShader(), NULL, 0);
	dc->PSSetShader(m_toonPS.GetShader(), NULL, 0);
	m_shadowmap.BindSRV(dc, 10);
	if (mp_cam->GetOffset().z != 0.f)
		m_playermodel.Draw();

	for (int i = 0; i < modelAmount; i++)
	{
		m_models[i]->UpdateTextureScroll(deltatime);
		m_models[i]->Draw();
	}
	dc->PSSetShader(m_3dnoLightps.GetShader(), NULL, 0);

	if (mp_cam->GetOffset().z == 0.f && im.enableHandModel)
	{
		dc->ClearDepthStencilView(m_dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
		m_viewmdl.Draw();
	}
	m_bulb.Draw();
	m_spotlight.Draw();
	m_spotlight.SetPosition(im.sl.position[0], im.sl.position[1], im.sl.position[2]);
	m_spotlight.SetRotation(im.sl.rotation[0], im.sl.rotation[1], im.sl.rotation[2]);
	m_shadowmap.DrawModel();
	//m_transformBuffer.Data().world = d::XMMatrixTranspose(d::XMMatrixTranslation(0.f, 0.f, 1.f));
	//m_transformBuffer.UpdateCB();
	//m_renderbox.Draw(dc);

	ImGuiRender();
	m_swapChain->Present((UINT)im.useVsync, NULL);
	//dc->ClearState();
}

ID3D11Device* DX11Addon::GetDevice() const
{
	return device;
}

ID3D11DeviceContext* DX11Addon::GetDeviceContext() const
{
	return dc;
}

void DX11Addon::ShutDown()
{
	DestroyWindow(*m_pHWND);
}

void RecursiveRead(Sceneheaders& header, ModelList& v, std::ifstream& reader)
{
	reader.read((char*)&header, 4);
	switch (header)
	{
	case Sceneheaders::eMODEL:
	{
		ModelStruct ms;
		reader.read((char*)&ms, sizeof(ModelStruct));
		if (ms.modelname[0] == '(')
		{
			std::string strCopy(ms.modelname);
			strCopy = strCopy.substr(3);
			sprintf_s(ms.modelname, strCopy.c_str());
		}

		v.emplace_back(new Model);
		v[v.size() - 1]->Init(std::string(ms.modelname));
		v[v.size() - 1]->SetPosition(ms.position);
		v[v.size() - 1]->SetRotation(ms.rotation);
		v[v.size() - 1]->SetScale(ms.scale);
		if (std::string(ms.mtrlname) != "")
			v[v.size() - 1]->GetMaterial().ImportMaterial(std::string(ms.mtrlname));

		for (UINT i = 1; i < ms.noOfExtraMats + 1 && ms.noOfExtraMats < 100; i++)
		{
			reader.read((char*)&ms.mtrlname, 24);
			if (ms.mtrlname != std::string(""))
				v[v.size() - 1]->GetMaterial(i).ImportMaterial(std::string(ms.mtrlname));
		}
		RecursiveRead(header, v, reader);
		break;
	}
	default:
		break;
	}
}
