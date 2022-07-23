#include "DX11Wrapper.h"
#include"../WindowWrap.h"

DX11Addon::DX11Addon(Window& window, Camera& camera) :
	m_width(window.getWinWidth()), m_height(window.getWinHeight()), m_pHWND(&window.getHWND())
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
	m_renderbox.Init(device);
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
	ID3D11Texture2D* backBuffer;

	HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	COM_ERROR(hr, "Get Buffer failed");

	hr = device->CreateRenderTargetView(backBuffer, NULL, &m_rtv);
	COM_ERROR(hr, "RTV failed");
	backBuffer->Release();

	return true;
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
	//rastDesc.FrontCounterClockwise = false;

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
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	D3D11_INPUT_ELEMENT_DESC lineLayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA},
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
	dc->PSSetSamplers(0, 1, &m_sampState);
	dc->OMSetRenderTargets(1, &m_rtv, m_dsView);
	dc->OMSetBlendState(m_blendState, NULL, 0xFFFFFFFF);

	ResourceHandler::AddTexture("goalflag.png"); // setting missingtexture
	ResourceHandler::AddTexture("ZANormal.png"); // Load LightWarp Texture
	dc->PSSetShaderResources(0, 1, ResourceHandler::GetTexture(1).GetSRVAdress());

	m_rLine.Init(device, dc);
	m_sphere.Init(device, dc);

	//ImportScene("Scenes\\ball.ptscene");

	//m_playermodel.Init("dirCapsule.obj");
	//m_playermodel.SetScale(.1f);

	NewScene();

	m_bulb.Init("bulb.obj", ModelType::eDEBUG);
	m_bulb.SetScale(1.2f);
	m_viewmdl.Init("handmodel2.obj", mp_cam);
	m_bulb.SetMaterialBuffer(m_materialBuffer);

	m_viewmdl.m_model.SetDCandBuffer(dc, m_transformBuffer);
	m_viewmdl.m_model.SetMaterialBuffer(m_materialBuffer);
	m_bulb.SetDCandBuffer(dc, m_transformBuffer);
	m_bulb.SetMaterialBuffer(m_materialBuffer);

	dc->VSSetConstantBuffers(0, 1, m_transformBuffer.GetReference());

	for (int i = 0; i < m_models.size(); i++)
	{
		m_models[i].SetMaterialBuffer(m_materialBuffer);
		m_models[i].SetDCandBuffer(dc, m_transformBuffer);
	}

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

	m_transformBuffer.Data().viewProj = d::XMMatrixTranspose(mp_cam->GetViewM() * mp_cam->GetProjM());
	m_lightbuffer.Data().direction = sm::Vector3(0.f, 1.f, 0.f);
	m_lightbuffer.Data().pointLightPosition = sm::Vector3(im.pointLightPos[0], im.pointLightPos[1], im.pointLightPos[2]);
	//m_lightbuffer.Data().forwardDir = mp_cam->GetForwardVector();
	m_lightbuffer.Data().camPos = { mp_cam->GetPosition().x, mp_cam->GetPosition().y, mp_cam->GetPosition().z, 1.f };
	m_lightbuffer.UpdateBuffer();

	//m_playermodel.SetRotation(-mp_cam->GetRotation().x, mp_cam->GetRotation().y, -mp_cam->GetRotation().z);
	//m_playermodel.Rotate(0.f, d::XM_PI, 0.f);
	//m_playermodel.SetPosition(mp_cam->GetPosition() + sm::Vector3(0.f, -0.1f, 0.f));
	//m_model.Rotate(0.f, 2.f * deltatime, 0.f);
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
			unsigned char data[255];
			ExportToon(im.buffer, data, im.gradientOffset, im.gradient[0], im.gradient[1]);
		}
	}

	ImGui::End();
}

void LoadButton(Material* pMaterial, std::string name, TextureType e)
{
	name += pMaterial->GetMapName(e);
	ImGui::Text(name.c_str());
	ImGui::SameLine();
	std::string buttonName = "Load##" + name;
	if (ImGui::Button(buttonName.c_str()))
	{
		std::string newMtrlString = Dialogs::OpenFile("Texture (*.png)\0*.png;*.jpg\0", "Assets\\Textures\\", 2);
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
	ImGui::Begin("Debug");

	ImGui::Checkbox("Demo window", &im.showDemoWindow);
	ImGui::Text("Press \"Q\" to lock/unlock mouse");

	if (ImGui::IsWindowHovered())
		m_isHoveringWindow = true;


	std::string fpsString = "FPS: " + std::to_string(m_fps);
	ImGui::Text(fpsString.c_str());

	if (ImGui::SliderInt("FOV", &im.fov, 40.f, 110.f))
		mp_cam->SetPerspective(im.fov, (float)m_width / (float)m_height, .1f, 100.f);

	ImGui::Checkbox("Vsync", &im.useVsync);
	ImGui::SameLine();
	ImGui::Checkbox("Handmodel", &im.enableHandModel);
	if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragFloat4("Ambient", im.ambient, 0.002f, 0.f, 1.f);
		ImGui::DragFloat3("Pointlight Position", im.pointLightPos, 0.01f);
		ImGui::DragFloat3("PointLightColor", im.pointLightColor, 0.01f, 0.f, 1.f);
		ImGui::SliderFloat("PointLight Strength", &im.pointLightStr, 0.f, 3.f);
		ImGui::SliderFloat("Specular power", &im.specPow, 1.f, 1000.f);
		ImGui::SliderFloat("Distance", &im.pointLightDistance, 0.f, 100.f);
		m_lightbuffer.Data().pointLightDistance = im.pointLightDistance;
		m_lightbuffer.Data().specularInstensity = im.specPow;

		ImGui::SliderFloat("Attenuation", &im.atten, 0.f, 10.f);
		m_lightbuffer.Data().atten = im.atten;
	}


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

	m_isHoveringWindow = false;
	ImguiDebug();
	ImGuiMenu();

	//ImGuiGradientWindow();
	ImGuiEntList();


	if (im.showDemoWindow)
		ImGui::ShowDemoWindow();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

#define PRINTER(name) printer(#name, )

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
				m_models.clear();
				ResourceHandler::ResetUses();
				NewScene();
			}
			if (ImGui::MenuItem("Open Scene...", NULL, false, true))
			{
				std::string diapath = Dialogs::OpenFile("Scene (*.ptscene)\0*.ptscene\0)", "Scenes\\");
				if (diapath != "")
				{
					m_models.clear();
					ResourceHandler::ResetUses();
					ImportScene(diapath);
					for (int i = 0; i < m_models.size(); i++)
					{
						m_models[i].SetDCandBuffer(dc, m_transformBuffer);
						m_models[i].SetMaterialBuffer(m_materialBuffer);
					}
				}
			}
			if (ImGui::MenuItem("Save scene as..."))
			{
				std::string diapath = Dialogs::SaveFile("Scene (*.ptscene)\0*.ptscene\0)", "Scenes\\");
				if (diapath != "")
				{
					if (StringHelper::GetExtension(diapath) != "ptscene")
						diapath += ".ptscene";
					ExportScene(diapath);
				}
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void DX11Addon::ImGuiEntList()
{
	ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Model List##2", (bool*)false/*, /*ImGuiWindowFlags_MenuBar*/))
	{
		if (ImGui::IsWindowHovered())
			m_isHoveringWindow = true;
		if (ImGui::Button(" + ##AddModel"))
		{
			std::string path = Dialogs::OpenFile("Model (*.obj)\0*.obj;*.txt\0", "Assets\\models\\");
			if (!path.empty())
			{
				Model newModel;
				path = StringHelper::GetName(path);
				newModel.Init(path);
				newModel.SetDCandBuffer(dc, m_transformBuffer);
				newModel.SetMaterialBuffer(m_materialBuffer);
				m_models.emplace_back(newModel);
				m_selected = m_models.size() - 1;
			}
		}
		//ImGui::SameLine();
		//if (mp_kb->IsKeyDown(Key::Q))
		//	m_selected = -1;

		ImGui::BeginChild("Lefty", ImVec2(150, 350), true);
		for (int i = 0; i < m_models.size(); i++)
		{
			if (ImGui::Selectable(m_models[i].GetName().c_str(), m_selected == i))
				m_selected = i;
			//if(ImGui::GetMouseClickedCount(ImGuiMouseButton))
			//if (ImGui::BeginPopupContextWindow())
			//{
			//	//ImGui::Text("dun dun duun");
			//	m_selected = -1;
			//	ImGui::EndPopup();
			//	ImGui::SetWindowFocus();
			//}
		}
		ImGui::EndChild();
		ImGui::End();


		/*ImGui::SameLine();*/
		if (m_selected != -1)
		{
			ImGui::Begin("Model properties");
			Model* pSelectedModel = &m_models[m_selected];
			Material* pMaterial = &pSelectedModel->GetMaterial();
			if (ImGui::IsWindowHovered())
				m_isHoveringWindow = true;

			ImGui::BeginGroup();
			ImGui::Text(m_models[m_selected].GetName().c_str());
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
			if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
			{
				LoadButton(pMaterial, "Diffuse: ", eDiffuse);
				LoadButton(pMaterial, "NormalMap: ", eNormal);
				LoadButton(pMaterial, "DistMap: ", eDistortion);
				LoadButton(pMaterial, "OpacityMap: ", eOpacity);

				sm::Vector2 diffuseSpeed(pMaterial->GetDiffuseScrollSpeed());
				sm::Vector2 distortionSpeed(pMaterial->GetDistortionScrollSpeed());
				bool hasDistMap = pMaterial->HasTexture(eDistortion);

				float diffSpeed[2]{ diffuseSpeed.x, diffuseSpeed.y };
				float distSpeed[2]{ distortionSpeed.x, distortionSpeed.y };

				ImGui::PushItemWidth(m_width * 0.14f);

				ImGui::SliderFloat2("Diffuse Scroll", diffSpeed, -.5f, .5f);
				if (hasDistMap)
					ImGui::SliderFloat2("Distortion Scroll", distSpeed, -.5f, .5f);
				if (ImGui::Button("Reset Scrollspeed"))
				{
					for (int i = 0; i < 2; i++)
					{
						diffSpeed[i] = 0.f;
						distSpeed[i] = 0.f;
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset scrollvalues"))
					pMaterial->ResetScrollValue();

				pMaterial->SetDiffuseScrollSpeed(diffSpeed[0], diffSpeed[1]);
				pMaterial->SetDistortionScrollSpeed(distSpeed[0], distSpeed[1]);

				if (hasDistMap)
				{
					int distDivider = pMaterial->GetDistortionDivider();
					ImGui::SliderInt("Dist divider", &distDivider, 1, 20);
					pMaterial->SetDistortionDivider(distDivider);
					float distScale = pMaterial->GetTextureScaleDist();
					ImGui::SliderFloat("Distortion scale", &distScale, 1.f, 10.f);
					pMaterial->SetTextureScaleDist(distScale);
				}

				float transparancy = pMaterial->GetTransparancy();
				ImGui::SliderFloat("Transparancy", &transparancy, 0.f, 1.f);
				pMaterial->SetTransparency(transparancy);

				float tiling = pMaterial->GetTextureScale();
				ImGui::SliderFloat("Tiling", &tiling, 1.f, 10.f);
				pMaterial->SetTextureScale(tiling);

				if (ImGui::Button("Import Material"))
				{
					std::string savePath = "";
					savePath = Dialogs::OpenFile("Material (*.pmtrl)\0*.pmtrl\0", "Assets\\pmtrl\\");

					if (savePath != "")
					{
						std::string test = StringHelper::GetExtension(savePath);
						if (test != "pmtrl")
							savePath += ".pmtrl";
						pMaterial->ImportMaterial(StringHelper::GetName(savePath));
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Export Material"))
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
			ImGui::EndGroup();
			if (mp_kb->IsKeyDown(Key::DELETEKEY))
			{
				m_models.erase(m_models.begin() + m_selected);
				m_selected = -1;
			}

			ImGui::End();
		}
		//for (int i = 0; i < m_models.size(); i++)
		//{
		//	m_models[i].GetMaterial().SetSelection(false);
		//}
		//if (m_selected != -1)
		//	m_models[m_selected].GetMaterial().SetSelection(true);
	}

}

void DX11Addon::ImportScene(std::string path)
{
	std::ifstream reader(path, std::ios::binary | std::ios::in);
	if (!reader.is_open())
		Popup::Error("Failed open scene");
	Sceneheaders header = Sceneheaders::enull;

	RecursiveRead(header, m_models, reader);
	reader.close();
}

void DX11Addon::ExportScene(std::string path)
{
	std::ofstream writer(path, std::ios::binary | std::ios::out);

	Sceneheaders header = Sceneheaders::eMODEL;
	for (int i = 0; i < m_models.size(); i++)
	{
		ModelStruct ms;
		strcpy_s(ms.modelname, m_models[i].GetName().c_str());
		if (ms.modelname[0] == '(')
		{
			std::string strCopy(ms.modelname);
			strCopy = strCopy.substr(3);
			sprintf_s(ms.modelname, strCopy.c_str());
		}
		ms.scale = m_models[i].GetScale();
		ms.rotation = m_models[i].GetRotation();
		ms.position = m_models[i].GetPosition();
		strcpy_s(ms.mtrlname, m_models[i].GetMaterial().GetFileName().c_str());
		writer.write((const char*)&header, 4);
		writer.write((const char*)&ms, sizeof(ModelStruct));
	}
	header = Sceneheaders::enull;
	writer.write((const char*)&header, 4);
	writer.close();
}

void DX11Addon::NewScene()
{
	m_selected = -1;
	Model groundPlane;
	groundPlane.Init("scaledplane.obj");
	groundPlane.SetDCandBuffer(dc, m_transformBuffer);
	groundPlane.SetMaterialBuffer(m_materialBuffer);
	groundPlane.Scale(10.f);
	groundPlane.SetPosition(0.f, -.5f, 0.f);
	m_models.emplace_back(groundPlane);
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

int ClickFoo(const sm::Ray& ray, std::vector<Model>& models)
{
	float distance = D3D11_FLOAT32_MAX;
	int index = -1;

	d::BoundingSphere transformedSphere;
	for (int i = 0; i < models.size(); i++)
	{
		transformedSphere = models[i].GetBSphere();
		sm::Vector3 center = transformedSphere.Center;
		//sm::Vector3 extents = transformedBox.Extents;
		float radius = transformedSphere.Radius;
		//if (extents.y == 0.f) extents.y = 0.01f;
		center += models[i].GetPosition();
		radius *= GetHighestValue(models[i].GetScale());

		transformedSphere.Center = center;
		transformedSphere.Radius = radius;
		//transformedBox.Extents = extents;

		float localDistance = 0.f;
		if (ray.Intersects(transformedSphere, distance))
		{
			if (localDistance <= distance)
			{
				distance = localDistance;
				index = i;
			}
		}
	}
	return index;
}

void DX11Addon::Click(const sm::Vector3& dir)
{
	sm::Ray ray;
	ray.position = mp_cam->GetPosition();
	ray.direction = dir;
	sm::Vector3 endPos = dir + mp_cam->GetPosition();
	m_rLine.SetLine(mp_cam->GetPosition(), endPos);

	int n = ClickFoo(ray, m_models);
	if (!m_isHoveringWindow)
		m_selected = n;
}

void DX11Addon::SetInputP(KeyboardHandler& kb)
{
	mp_kb = &kb;
}

void DX11Addon::Render(const float& deltatime)
{
	int modelAmount = m_models.size();
	float bgColor[] = { .1f,.1f,.1f,1.f };
	dc->ClearRenderTargetView(m_rtv, bgColor);
	dc->ClearDepthStencilView(m_dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	dc->OMSetBlendState(m_blendState, NULL, 0xFFFFFFFF);

	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	dc->IASetInputLayout(m_lineVS.GetInputLayout());
	dc->VSSetShader(m_lineVS.GetShader(), NULL, 0);
	dc->PSSetShader(m_linePS.GetShader(), NULL, 0);

	if (m_selected != -1)
	{

		//m_transformBuffer.Data().world = d::XMMatrixTranspose(d::XMMatrixTranslation(0.f, 1.f, 0.f));
		d::BoundingSphere sphere;
		sphere = m_models[m_selected].GetBSphere();
		float radius = sphere.Radius * GetHighestValue(m_models[m_selected].GetScale());
		radius *= 2;
		//if (extents.y == 0.f) extents.y = 0.01f;
		sm::Vector3 position = m_models[m_selected].GetPosition() + sphere.Center;
		sm::Matrix boxMatrix = d::XMMatrixTranspose(d::XMMatrixScaling(radius, radius, radius) * d::XMMatrixTranslationFromVector(position));
		m_transformBuffer.Data().world = boxMatrix;
		m_transformBuffer.UpdateBuffer();
		m_sphere.Draw(dc);
	}
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	//m_transformBuffer.Data().world = d::XMMatrixIdentity();
	//m_transformBuffer.UpdateBuffer();
	//m_rLine.Draw(dc);
	dc->IASetInputLayout(m_3dvs.GetInputLayout());
	dc->VSSetShader(m_3dvs.GetShader(), NULL, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//dc->PSSetShader(m_3dps.GetShader(), NULL, 0);
	dc->PSSetShader(m_toonPS.GetShader(), NULL, 0);

	//if (mp_cam->GetOffset().z != 0.f)
	//	m_playermodel.Draw();

	for (int i = 0; i < modelAmount; i++)
	{
		m_models[i].UpdateTextureScroll(deltatime);
		m_models[i].Draw();
	}
	//m_water.Draw();
	dc->PSSetShader(m_3dnoLightps.GetShader(), NULL, 0);

	if (mp_cam->GetOffset().z == 0.f && im.enableHandModel)
	{
		dc->ClearDepthStencilView(m_dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
		m_viewmdl.Draw();
	}
	m_bulb.Draw();





	//m_transformBuffer.Data().world = d::XMMatrixTranspose(d::XMMatrixTranslation(0.f, 0.f, 1.f));
	//m_transformBuffer.UpdateCB();
	//m_renderbox.Draw(dc);



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

void RecursiveRead(Sceneheaders& header, std::vector<Model>& v, std::ifstream& reader)
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
		Model model;
		model.Init(std::string(ms.modelname));
		model.SetPosition(ms.position);
		model.SetRotation(ms.rotation);
		model.SetScale(ms.scale);
		if (std::string(ms.mtrlname) != "")
			model.GetMaterial().ImportMaterial(std::string(ms.mtrlname));
		v.emplace_back(model);
		RecursiveRead(header, v, reader);
		break;
	}
	default:
		break;
	}
}
