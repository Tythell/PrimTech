#include"pch.h"
#include "DX11Wrapper.h"
#include"Windows/WindowWrap.h"

#define shadowQuality 2

using namespace pt;

namespace PrimtTech
{
	DX11Renderer::DX11Renderer(Window& window) :
		m_width(window.getWinWidth()), m_height(window.getWinHeight()), m_pHWND(&window.getHWND()),
		m_shadowmap(1024 * shadowQuality, 1024 * shadowQuality), m_viewport(0.f, 0.f, (float)m_width, (float)m_height),
		m_shadowCam(0xffffffff)
	{
		m_pWin = &window;

		initSwapChain();
		initRTV();
		SetupDSAndVP();
		InitRastNSampState();
		InitBlendState();
		m_shadowmap.Init(device);

		ResourceHandler::SetDevice(device);

		InitShaders();
		InitConstantBuffers();

		m_shadowCam.SetOrtographic(1024 * shadowQuality, 1024 * shadowQuality, .1f, 25.f);

		InitScene();
		m_renderbox.Init(device, dc);
	}

	DX11Renderer::~DX11Renderer()
	{
		m_guiHandler->ImGuiShutDown();

		ResourceHandler::Unload();

		//m_transformBuffer.Release();
		//m_lightbuffer.Release();
		//m_materialBuffer.Release();

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
		m_wireFrameState->Release();
	}

	void DX11Renderer::SetImGuiHandler(ImGuiHandler& gui)
	{
		m_guiHandler = &gui;
		im = m_guiHandler->GetVarPtrs();
		im->width = m_width;
		im->height = m_height;
		m_guiHandler->SetBufferPtrs(m_lightbuffer, m_materialBuffer);
		//m_guiHandler->SetPtrs(mp_camHandler);
		m_guiHandler->ImGuiInit(m_pWin->getHWND(), device, dc);
	}

	bool DX11Renderer::initSwapChain()
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

	bool DX11Renderer::initRTV()
	{
		ID3D11Texture2D* backBuffer = nullptr;

		HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
		COM_ERROR(hr, "Get Buffer failed");

		hr = device->CreateRenderTargetView(backBuffer, NULL, &m_rtv);
		COM_ERROR(hr, "RTV failed");
		backBuffer->Release();

		return true;
	}

	bool DX11Renderer::SetupDSAndVP()
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

	bool DX11Renderer::InitRastNSampState()
	{
		D3D11_RASTERIZER_DESC rastDesc;
		ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
		rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;

		rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
		rastDesc.FrontCounterClockwise = false;

		HRESULT hr = device->CreateRasterizerState(&rastDesc, &m_rasterizerState);
		COM_ERROR(hr, "Rasterizer State setup failed");

		rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
		rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
		COM_ERROR(device->CreateRasterizerState(&rastDesc, &m_wireFrameState), "Failed to setup wireframestate");

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



		for (int i = 0; i < 4; i++)
			sampDesc.BorderColor[i] = 1.f;

		sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;

		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

		hr = device->CreateSamplerState(&sampDesc, &m_shadowSampler);
		COM_ERROR(hr, "Sampler State setup failed");
		//ID3D11SamplerState* samplers[] = {m_wrapSampler, m_clampSampler, m_shadowSampler};
		dc->PSSetSamplers(0, 1, &m_wrapSampler);
		dc->PSSetSamplers(1, 1, &m_clampSampler);
		dc->PSSetSamplers(2, 1, &m_shadowSampler);

		return true;
	}

	void DX11Renderer::InitBlendState()
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

	bool DX11Renderer::InitShaders()
	{
		D3D11_INPUT_ELEMENT_DESC layout3D[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,	 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
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

	bool DX11Renderer::InitScene()
	{
		dc->RSSetState(m_rasterizerState);
		dc->OMSetDepthStencilState(m_dsState, 0);

		std::vector<BBVertex> gridArr;

		// grid
		uint nLines = 11;
		gridArr.resize(nLines * 4);
		for (int i = 0; i < nLines; i++)
		{
			gridArr[2 * i + 0].m_position = sm::Vector3(i - 5, 0.f, (float)nLines / 2.f);
			gridArr[2 * i + 1].m_position = sm::Vector3(i - 5, 0.f, -(float)nLines / 2.f);
			gridArr[2 * i + 0].m_color = GRAY_3F;
			gridArr[2 * i + 1].m_color = GRAY_3F;
		}
		for (int i = 0; i < nLines; i++)
		{
			uint index = (nLines * 2) + i * 2;
			gridArr[index + 0].m_position = sm::Vector3(-(float)nLines / 2.f, 0.f, i - 5);
			gridArr[index + 1].m_position = sm::Vector3((float)nLines / 2.f, 0.f, i - 5);

			gridArr[index + 0].m_color = GRAY_3F;
			gridArr[index + 1].m_color = GRAY_3F;
		}



		m_grid.CreateVertexBuffer(device, gridArr.data(), gridArr.size());

		dc->OMSetBlendState(m_blendState, NULL, 0xFFFFFFFF);

		//ResourceHandler::AddTexture("missingTexture.png"); // setting missingtexture
		ResourceHandler::AddTexture(".NoTexture.pngg"); // setting missingtexture
		ResourceHandler::AddTexture("ZANormal.png"); // Load LightWarp Texture
		//ResourceHandler::AddTexture("ZATf2esk.png"); // Load LightWarp Texture

		//ResourceHandler::ReserveMeshMemory(6);

		dc->PSSetShaderResources(0, 1, ResourceHandler::GetTexture(1).GetSRVAdress());

		//m_rLine.Init(device, dc);
		//m_sphere.Init(device, dc, 8);

		//ImportScene("Scenes\\multsc.ptscene");

		/*m_playermodel.Init("dirCapsule.obj", ModelType::eUNSPECIFIED, false);
		m_playermodel.SetScale(.1f);

		m_camModel.Init("camera.obj", ModelType::eDEBUG, false);
		m_camModel.SetMaterialBuffer(m_materialBuffer);
		m_camModel.SetDCandBuffer(dc, m_transformBuffer);
		m_camModel.GetMaterial().SetTransparency(1.f);

		m_bulb.Init("bulb.obj", ModelType::eDEBUG);
		m_bulb.SetScale(1.2f);
		m_bulb.GetMaterial().SetTransparency(1.f);

		m_viewmdl.Init("handmodel2.obj", mp_camHandler);
		m_bulb.SetMaterialBuffer(m_materialBuffer);
		m_playermodel.SetMaterialBuffer(m_materialBuffer);
		m_playermodel.SetDCandBuffer(dc, m_transformBuffer);
		m_viewmdl.m_model.SetDCandBuffer(dc, m_transformBuffer);
		m_viewmdl.m_model.SetMaterialBuffer(m_materialBuffer);
		m_bulb.SetDCandBuffer(dc, m_transformBuffer);
		m_bulb.SetMaterialBuffer(m_materialBuffer);

		dc->VSSetConstantBuffers(0, 1, m_transformBuffer.GetReference());

		for (int i = 0; i < m_models.size(); i++)
		{
			m_models[i]->SetMaterialBuffer(m_materialBuffer);
			m_models[i]->SetDCandBuffer(dc, m_transformBuffer);
		}

		m_shadowmap.InitModel(dc, m_transformBuffer, m_materialBuffer);*/

		//m_renderbox.Init(device);
		return true;
	}

	void DX11Renderer::InitConstantBuffers()
	{
		m_transformBuffer.CreateConstantBuffer(device, dc);
		dc->VSSetConstantBuffers(0, 1, m_transformBuffer.GetReference());
		m_lightbuffer.CreateConstantBuffer(device, dc);
		dc->PSSetConstantBuffers(0, 1, m_lightbuffer.GetReference());
		m_materialBuffer.CreateConstantBuffer(device, dc);
		dc->PSSetConstantBuffers(1, 1, m_materialBuffer.GetReference());
	}

	void DX11Renderer::UpdateScene(const float& deltatime)
	{
		m_lightbuffer.Data().ambientColor = { im->ambient[0], im->ambient[1], im->ambient[2] };
		//m_lightbuffer.Data().ambientStr = 1.f;
		m_lightbuffer.Data().ambientStr = im->ambient[3];
		m_lightbuffer.Data().pointLightColor = { 1.f, 1.f, 1.f };
		m_lightbuffer.Data().pointlightStre = im->pointLightStr;
		m_lightbuffer.Data().cbShadowBias = im->shadowBias;
		m_lightbuffer.Data().pointLightDistance = 10.f;

		m_lightbuffer.Data().direction = sm::Vector3(0.f, 1.f, 0.f);
		m_lightbuffer.Data().pointLightPosition = sm::Vector3(im->pointLightPos[0], im->pointLightPos[1], im->pointLightPos[2]);
		//m_lightbuffer.Data().forwardDir = mp_cam->GetForwardVector();
		m_lightbuffer.Data().camPos = { 0.f,0.f,0.f,1.f/*mp_currentCam->GetPosition().x, mp_currentCam->GetPosition().y, mp_currentCam->GetPosition().z, 1.f*/ };
		m_lightbuffer.MapBuffer();


		//m_playermodel.SetRotation(0.f/*-mp_cam->GetRotation().x*/, mp_currentCam->GetRotation().y, 0.f);
		//m_playermodel.Rotate(0.f, d::XM_PI, 0.f);
		//m_playermodel.SetPosition(mp_currentCam->GetPositionNoOffset() + sm::Vector3(0.f, -0.1f, 0.f));
		//m_shadowmap.SetPos(mp_cam->GetPositionNoOffset());
		//m_model.Rotate(0.f, 2.f * deltatime, 0.f);
	}

	void DX11Renderer::ImGuiRender()
	{
		if (m_guiHandler)
		{
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
			ImGuizmo::BeginFrame();
			//m_isHoveringWindow = false;

			m_guiHandler->ImguiRender();

			if (im->showShadowMapDepth) ImGuTextureDisplay();

			//ImGuiGradientWindow();

			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}
	}

	void DX11Renderer::ExportScene(std::string path)
	{
		/*std::ofstream writer(path, std::ios::binary | std::ios::out);

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
		writer.write((const char*)&header, 1);
		writer.close();*/
	}

	void DX11Renderer::ImGuTextureDisplay()
	{
		float winvar = 400;
		float offset = 35;
		ImVec2 winSize(winvar, winvar + offset);
		ImGui::SetNextWindowSize(winSize);
		ImGui::Begin("Texture Display", &im->showShadowMapDepth, ImGuiWindowFlags_NoResize);

		//ImTextureID tex = m_shadowmap.GetSRV();
		//ImGui::Image(tex, { winSize.x, winSize.x });
		//if (ImGui::IsWindowHovered())
			//m_isHoveringWindow = true;

		ImGui::End();
	}
	using uchar = unsigned char;

	void DX11Renderer::SetLightWarp(const std::string& path)
	{
		int textureIndex = ResourceHandler::CheckTextureNameExists(StringHelper::GetName(path));
		if (textureIndex != -1)
			dc->PSSetShaderResources(0, 1, ResourceHandler::GetTexture(textureIndex).GetSRVAdress());
		else
		{
			TextureMap* tex = ResourceHandler::AddTexture(path);
			dc->PSSetShaderResources(0, 1, tex->GetSRVAdress());
		}
		ResourceHandler::GetTextureAdress(1)->CreateFromFile(path.c_str(), device);
		dc->PSSetShaderResources(0, 1, ResourceHandler::GetTexture(1).GetSRVAdress());
	}

	void DX11Renderer::Render(const float& deltatime)
	{
		float bgColor[] = { .1f,.1f,.1f,1.f };

		dc->ClearRenderTargetView(m_rtv, bgColor);
		dc->ClearDepthStencilView(m_dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
		dc->OMSetBlendState(m_blendState, NULL, 0xFFFFFFFF);

		dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		dc->RSSetState(m_rasterizerState);

		std::vector<MeshRef>& rMeshrefs = ComponentHandler::GetComponentArray<MeshRef>();
		std::vector<TransformComp>& rTransforms = ComponentHandler::GetComponentArray<TransformComp>();
		
		// shadow code here
		m_shadowmap.Bind(dc, 10);
		m_shadowmap.BindSRV(dc, 10);
		dc->PSSetShader(NULL, NULL, 0);

		uint numMEshRefs = (uint)rMeshrefs.size();
		uint offset = 0;

		for (int i = 0; i < numMEshRefs; i++)
		{
			uint entId = rMeshrefs[i].EntId();
			Mesh* meshPtr = rMeshrefs[i].GetMeshContainerP();

			//TransformComp* pTransformComp = &Entity::s_ents[entId]->Transform();
			TransformComp* pTransformComp = &rTransforms[entId];

			m_transformBuffer.Data().world = pTransformComp->GetWorldTransposed();
			m_transformBuffer.MapBuffer();

			dc->IASetVertexBuffers(0, 1, meshPtr->GetVBuffer().GetReference(), meshPtr->GetVBuffer().GetStrideP(), &offset);
			for (int j = 0; j < meshPtr->GetNofMeshes(); j++)
			{
				uint matIndex = rMeshrefs[i].GetMaterialIndex(j);
				Material& rMat = ResourceHandler::GetMaterial(matIndex);
				rMat.Set(dc, m_materialBuffer);
				rMat.UpdateTextureScroll(deltatime);

				int v1 = meshPtr->GetMeshOffsfets()[j + 1], v2 = meshPtr->GetMeshOffsfets()[j];
				dc->Draw(v1 - v2, v2);
			}
		}


		dc->OMSetRenderTargets(1, &m_rtv, m_dsView);
		dc->RSSetViewports(1, &m_viewport);

		dc->IASetInputLayout(m_lineVS.GetInputLayout());
		dc->VSSetShader(m_lineVS.GetShader(), NULL, 0);
		dc->PSSetShader(m_linePS.GetShader(), NULL, 0);

		pt::Camera& cc = ComponentHandler::GetComponentByIndex<pt::Camera>(m_activeCamIndex);
		
		m_transformBuffer.Data().viewProj = d::XMMatrixTranspose(cc.GetViewMatrix() * cc.GetProjMatrix());

		dc->IASetInputLayout(m_3dvs.GetInputLayout());
		dc->VSSetShader(m_3dvs.GetShader(), NULL, 0);
		dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		dc->PSSetShader(m_toonPS.GetShader(), NULL, 0);

		m_materialBuffer.Data().flags = 0;

		

		

		// iterate through meshrefs
		for (int i = 0; i < numMEshRefs; i++)
		{
			uint entId = rMeshrefs[i].EntId();
			Mesh* meshPtr = rMeshrefs[i].GetMeshContainerP();

			//TransformComp* pTransformComp = &Entity::s_ents[entId]->Transform();
			TransformComp* pTransformComp = &rTransforms[entId];

			m_transformBuffer.Data().world = pTransformComp->GetWorldTransposed();
			m_transformBuffer.MapBuffer();

			dc->IASetVertexBuffers(0, 1, meshPtr->GetVBuffer().GetReference(), meshPtr->GetVBuffer().GetStrideP(), &offset);
			for (int j = 0; j < meshPtr->GetNofMeshes(); j++)
			{
				uint matIndex = rMeshrefs[i].GetMaterialIndex(j);
				Material& rMat = ResourceHandler::GetMaterial(matIndex);
				rMat.Set(dc, m_materialBuffer);
				rMat.UpdateTextureScroll(deltatime);

				int v1 = meshPtr->GetMeshOffsfets()[j + 1], v2 = meshPtr->GetMeshOffsfets()[j];
				dc->Draw(v1 - v2, v2);
			}
		}

		// Draw grid
		m_transformBuffer.Data().world = d::XMMatrixIdentity();
		m_transformBuffer.MapBuffer();

		dc->IASetVertexBuffers(0, 1, m_grid.GetReference(), m_grid.GetStrideP(), &offset);
		dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
		dc->VSSetShader(m_lineVS.GetShader(), NULL, 0);
		dc->IASetInputLayout(m_lineVS.GetInputLayout());
		dc->PSSetShader(m_linePS.GetShader(), NULL, 0);
		dc->Draw(m_grid.GetBufferSize(), 0);

		std::vector<AABBComp>& rAabbs = ComponentHandler::GetComponentArray<AABBComp>();
		uint numAabbs = (uint)rAabbs.size();
		m_renderbox.SetBuffer(dc);
		for (int i = 0; i < numAabbs; i++)
		{
			uint entId = rAabbs[i].EntId();
			TransformComp& pTransformComp = rTransforms[entId];

			sm::Vector3 scaling = rAabbs[i].GetBox().Extents;

			d::XMMATRIX world = d::XMMatrixScalingFromVector(scaling) *
				d::XMMatrixTranslationFromVector(sm::Vector3(rAabbs[i].GetBox().Center));

			bool intersecting = rAabbs[i].IsIntersecting();
			if (intersecting) m_renderbox.SetColor(GREEN_3F);
			else m_renderbox.SetColor(WHITE_3F);
				
			

			m_transformBuffer.Data().world = d::XMMatrixTranspose(world);
			m_transformBuffer.MapBuffer();
			m_renderbox.DrawShape(dc);
		}

		ImGuiRender();
		m_swapChain->Present((UINT)im->useVsync, NULL);
	}

	ID3D11Device* DX11Renderer::GetDevice() const
	{
		return device;
	}

	ID3D11DeviceContext* DX11Renderer::GetDeviceContext() const
	{
		return dc;
	}

	void DX11Renderer::ShutDown()
	{
		DestroyWindow(*m_pHWND);
	}
}