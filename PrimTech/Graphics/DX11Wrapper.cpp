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
	InitBlendState();

	ResourceHandler::SetDevice(device);

	InitShaders();
	InitConstantBuffers();
	InitScene();

	ImGuiInit(window.getHWND());
}

DX11Addon::~DX11Addon()
{
	ImGuiShutDown();

	delete im.buffer;

	ResourceHandler::Unload();

	device->Release();
	dc->Release();
	m_swapChain->Release();
	m_rtv->Release();
	m_depthStencilBuffer->Release();
	m_dsView->Release();
	m_dsState->Release();
	m_rasterizerState->Release();
	m_sampState->Release();
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
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

	HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	COM_ERROR(hr, "Get Buffer failed");

	hr = device->CreateRenderTargetView(backBuffer.Get(), NULL, &m_rtv);
	COM_ERROR(hr, "RTV failed");



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

	CD3D11_VIEWPORT viewport(0.f, 0.f, (float)m_width, (float)m_height);

	dc->RSSetViewports(1, &viewport);

	return SUCCEEDED(hr);
}

bool DX11Addon::InitRastNSampState()
{
	D3D11_RASTERIZER_DESC rastDesc;
	ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
	rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;

	HRESULT hr = device->CreateRasterizerState(&rastDesc, &m_rasterizerState);
	COM_ERROR(hr, "Rasterizer State setup failed");

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));
	//sampDesc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = device->CreateSamplerState(&sampDesc, &m_sampState);
	COM_ERROR(hr, "Sampler State setup failed");

	sampDesc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;

	return true;
}

void DX11Addon::InitBlendState()
{
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(D3D11_RENDER_TARGET_BLEND_DESC));
	rtbd.BlendEnable = true;
	//rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA_SAT;
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
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	m_3dvs.Init(device, "../x64/Debug/vertexshader.cso");
	m_3dvs.InitInputLayout(device, layout3D, ARRAYSIZE(layout3D));
	m_3dps.Init(device, "../x64/Debug/BasePS.cso");
	m_3dnoLightps.Init(device, "../x64/Debug/NoLightPs.cso");
	m_toonPS.Init(device, "../x64/Debug/ToonPS.cso");

	dc->VSSetShader(m_3dvs.GetShader(), NULL, 0);
	dc->PSSetShader(m_3dps.GetShader(), NULL, 0);

	return true;
}

bool DX11Addon::InitScene()
{
	dc->IASetInputLayout(m_3dvs.GetInputLayout());
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dc->RSSetState(m_rasterizerState);
	dc->OMSetDepthStencilState(m_dsState, 0);
	dc->PSSetSamplers(0, 1, &m_sampState);
	dc->OMSetRenderTargets(1, &m_rtv, m_dsView);
	dc->OMSetBlendState(m_blendState, NULL, 0xFFFFFFFF);

	ResourceHandler::AddTexture("goalflag.png"); // setting missingtexture
	ResourceHandler::AddTexture("ZAToon.png");
	dc->PSSetShaderResources(2, 1, ResourceHandler::GetTexture(1).GetSRVAdress());
	
	m_model.Init("scuffball.obj");
	m_model.SetPosition(0.f, 0.f, 3.f);
	m_model.SetMaterialBuffer(m_materialBuffer);

	m_bulb.Init("bulb.obj");
	m_bulb.SetMaterialBuffer(m_materialBuffer);
	m_bulb.SetScale(1.2f);

	m_plane.Init("plane.txt");
	m_plane.LoadTexture("Brick_Diffuse.jpg");
	m_plane.SetPosition(0.f, -1.f, 0.f);
	m_plane.SetScale(10.f);
	m_plane.SetMaterialBuffer(m_materialBuffer);

	m_plane.setDiffuseScrollSpeed(-0.1f, -0.1f);

	m_playermodel.Init("dirCapsule.obj");
	m_playermodel.SetScale(.1f);
	m_playermodel.SetMaterialBuffer(m_materialBuffer);

	m_gunter.Init("gunter.obj");
	m_gunter.LoadTexture("gunteruv.png", eDiffuse);
	m_gunter.SetPosition(-1.f, 2.f, -6.f);
	m_gunter.SetRotation(0.f, d::XM_PI, 0.f);
	m_gunter.SetMaterialBuffer(m_materialBuffer);

	m_menacing.Init("menacing.obj");
	m_menacing.SetPosition(-3.f, 2.f, -6.f);
	m_menacing.SetRotation(0.f, d::XM_PI, 0.f);
	m_menacing.SetMaterialBuffer(m_materialBuffer);
	m_handmodel.Init("handmodel2.obj");
	m_handmodel.SetScale(.1f);
	m_handmodel.SetMaterialBuffer(m_materialBuffer);

	m_water.Init("plane.txt");
	m_water.SetScale(4.f);
	m_water.LoadTexture("water.png");
	m_water.LoadTexture("waterDist.png", eDistortion);
	m_water.SetPosition(-4.f, 0.f, 0.f);
	m_water.SetMaterialBuffer(m_materialBuffer);
	m_water.GetMaterial().SetTransparency(.7f);


	dc->VSSetConstantBuffers(0, 1, m_transformBuffer.GetReference());
	AllModels::Addbuffers(dc, m_transformBuffer);
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
	m_lightbuffer.Data().ambientLight = { im.ambient[0], im.ambient[1], im.ambient[2] };
	m_lightbuffer.Data().ambientStr = im.ambient[3];
	m_lightbuffer.Data().pointLightColor = { im.pointLightColor[0], im.pointLightColor[1], im.pointLightColor[2] };
	m_lightbuffer.Data().pointlightStr = im.pointLightStr;

	m_transformBuffer.Data().viewProj = d::XMMatrixTranspose(mp_cam->GetViewM() * mp_cam->GetProjM());
	m_lightbuffer.Data().direction = sm::Vector3(0.f, 1.f, 0.f);
	m_lightbuffer.Data().position = sm::Vector3(im.pointLightPos[0], im.pointLightPos[1], im.pointLightPos[2]);
	m_lightbuffer.UpdateCB();

	m_playermodel.SetRotation(-mp_cam->GetRotation().x, mp_cam->GetRotation().y, -mp_cam->GetRotation().z);
	m_playermodel.Rotate(0.f, d::XM_PI, 0.f);
	m_playermodel.SetPosition(mp_cam->GetPosition() + sm::Vector3(0.f, -0.1f, 0.f));
	m_model.Rotate(0.f, 2.f * deltatime, 0.f);

	m_materialBuffer.Data().distDiv = im.distDiv;
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


	//ImGui::Text("Gradients");
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImVec2 full_gradient_size = ImVec2(255, ImGui::GetFrameHeight());
	float halfPoint = full_gradient_size.x / 2;
	halfPoint += im.gradientOffset;
	//ImVec2 gradient_size = ImVec2(im.gradient[1], ImGui::GetFrameHeight());
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
			unsigned char data[255];
			ExportToon(im.buffer, data, im.gradientOffset, im.gradient[0], im.gradient[1]);
		}
	}

	ImGui::End();
}

void DX11Addon::ImGuiRender()
{
	static bool im_appear = true;
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_NoCollapse);
	ImGui::Checkbox("Demo window", &im.showDemoWindow);
	ImGui::Text("Press Q to lock/unlock mouse");

	std::string fpsString = "FPS: " + std::to_string(m_fps);
	ImGui::Text(fpsString.c_str());

	if (ImGui::SliderInt("FOV", &im.fov, 40.f, 110.f))
		mp_cam->SetPerspective(im.fov, (float)m_width / (float)m_height, .1f, 100.f);

	ImGui::Checkbox("Vsync", &im.useVsync);
	ImGui::SameLine();
	ImGui::Checkbox("Handmodel", &im.enableHandModel);
	ImGui::DragFloat4("Ambient", im.ambient, 0.002f, 0.f, 1.f);
	ImGui::DragFloat3("Pointlight Position", im.pointLightPos, 0.01f);
	ImGui::DragFloat3("PointLightColor", im.pointLightColor, 0.01f, 0.f, 1.f);
	ImGui::SliderFloat("PointLight Strength", &im.pointLightStr, 0.f, 3.f);
	const float scrollmax = .5f;
	ImGui::SliderFloat2("Diffuse scroll", im.diffuseScrollSpeed, -scrollmax, scrollmax);
	ImGui::SliderFloat2("Distortion scroll", im.distScrollSpeed, -scrollmax, scrollmax);
 	m_water.setDiffuseScrollSpeed(im.diffuseScrollSpeed[0], im.diffuseScrollSpeed[1]);
	m_water.GetMaterial().SetDistortionScrollSpeed(im.distScrollSpeed[0], im.distScrollSpeed[1]);
	
	if (ImGui::Button("Reset Scrolling"))
	{
		for (int i = 0; i < 2; i++)
		{
			im.diffuseScrollSpeed[i] = 0.f;
			im.distScrollSpeed[i] = 0.f;
		}
	}
	ImGui::SliderInt("Distortion divider", &im.distDiv, 1, 20);
	std::string camoffsetString = "Camera offset: ";
	camoffsetString += std::to_string(mp_cam->GetOffset().z);
	ImGui::Text(camoffsetString.c_str());
	//if (ImGui::Button("Focus pointLight"))
		//mp_cam->SetPosition(im.pointLightPos[0], im.pointLightPos[1], im.pointLightPos[2]);
	
	m_bulb.SetPosition(im.pointLightPos[0], im.pointLightPos[1], im.pointLightPos[2]);

	ImGui::Text(GetVectorAsString(mp_cam->GetRotation()).c_str());

	//ImGui::ShowDemoWindow();

	ImGui::End();;

	ImGuiGradientWindow();

	if(im.showDemoWindow)
		ImGui::ShowDemoWindow();


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

void DX11Addon::SetInputP(KeyboardHandler& kb)
{
	mp_kb = &kb;
}

void DX11Addon::Render(const float& deltatime)
{
	float bgColor[] = { .1f,.1f,.1f,1.f };
	dc->ClearRenderTargetView(m_rtv, bgColor);
	dc->ClearDepthStencilView(m_dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	dc->OMSetBlendState(m_blendState, NULL, 0xFFFFFFFF);

	//dc->PSSetShader(m_3dps.GetShader(), NULL, 0);
	m_gunter.Draw();
	m_model.Draw();
	m_plane.Draw();
	
	if (mp_cam->GetOffset().z != 0.f)
		m_playermodel.Draw();

	//m_water.UpdateTextureScroll(deltatime);
	//m_water.Draw();
	dc->PSSetShader(m_toonPS.GetShader(), NULL, 0);
	m_menacing.Draw();
	m_bulb.Draw();
	
	if (mp_cam->GetOffset().z == 0.f && im.enableHandModel)
	{
		dc->ClearDepthStencilView(m_dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
		m_handmodel.SetPosition(mp_cam->GetPosition());
		m_handmodel.SetRotation(-mp_cam->GetRotation().x, mp_cam->GetRotation().y + d::XM_PI, -mp_cam->GetRotation().z);
		m_handmodel.Draw();
	}

	ImGuiRender();
	m_swapChain->Present((UINT)im.useVsync, NULL);
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
