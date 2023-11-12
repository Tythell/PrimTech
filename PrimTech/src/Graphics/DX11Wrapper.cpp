#include"pch.h"
#include "DX11Wrapper.h"
#include"Windows/WindowWrap.h"

#define shadowQuality 2

using namespace pt;

namespace PrimtTech
{
	Renderer::Renderer(Window& window) :
		m_width(window.getWinWidth()), m_height(window.getWinHeight()), m_pHWND(&window.getHWND()),
		m_shadowmap(1024 * shadowQuality, 1024 * shadowQuality), m_viewport(0.f, 0.f, (float)m_width, (float)m_height)
	{
		m_pWin = &window;

		initSwapChain();
		initRTV();
		SetupDSAndVP();
		InitRastNSampState();
		InitBlendState();
		m_shadowmap.Init(device);

		ResourceHandler::SetDevice(device, dc);

		InitShaders();
		InitConstantBuffers();

		pt::Entity& shadowEnt = pt::Entity::Create();
		pt::Camera* pScam = shadowEnt.AddComponent<pt::Camera>();
		pScam->SetOrtographic(20.f, 20.f, .1f, 10.f);
		//pScam->SetRotationOffset(1.58f, 0.f, 0.f);
		m_shadowCamIndex = shadowEnt.GetComponentIndex<Camera>();

		shadowEnt.SetPosition(0.f, 4.f, 0.f);
		shadowEnt.SetRotation(1.58f, 0.f, 0.f);
		pScam->UpdateView(shadowEnt.Transform());

		InitScene();
		m_renderbox.Init(device, dc);

		m_lightVector.resize(32);

		m_multiLightBuffer.CreateStructuredBuffer(device, m_lightVector.data(), m_lightVector.size(), dc);

		m_multiLightBuffer.BindSRV(11);
	}

	Renderer::~Renderer()
	{
		//mp_clickbufferSrv->Release();
		m_guiHandler->ImGuiShutDown();

		ResourceHandler::Unload();

		m_transformBuffer.Release();
		m_lightbuffer.Release();
		m_materialBuffer.Release();
		m_multiLightBuffer.Release();
		m_grid.Release();

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

	void Renderer::SetImGuiHandler(ImGuiHandler& gui)
	{
		m_guiHandler = &gui;
		im = m_guiHandler->GetVarPtrs();
		im->width = m_width;
		im->height = m_height;
		m_guiHandler->ImGuiInit(m_pWin->getHWND(), device, dc);
	}

	bool Renderer::initSwapChain()
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

	bool Renderer::initRTV()
	{
		ID3D11Texture2D* backBuffer = nullptr;

		HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
		COM_ERROR(hr, "Get Buffer failed");

		hr = device->CreateRenderTargetView(backBuffer, NULL, &m_rtv);
		COM_ERROR(hr, "RTV failed");
		backBuffer->Release();

		return true;
	}

	bool Renderer::SetupDSAndVP()
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
		//dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		hr = device->CreateDepthStencilState(&dssDesc, &m_dsState);
		COM_ERROR(hr, "DS State failed");

		return SUCCEEDED(hr);
	}

	bool Renderer::InitRastNSampState()
	{
		D3D11_RASTERIZER_DESC rastDesc;
		ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
		rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;

		//rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
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

	void Renderer::InitBlendState()
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

	bool Renderer::InitShaders()
	{
		D3D11_INPUT_ELEMENT_DESC layout3D[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,	 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BITANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"INSTWORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"INSTWORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"INSTWORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"INSTWORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		};

		D3D11_INPUT_ELEMENT_DESC lineLayout[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA},
			{"COLOR",	 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA},
		};

		m_3dvs.Init(device, "vertexshader.cso");
		m_3dvs.InitInputLayout(device, layout3D, ARRAYSIZE(layout3D));
		m_toonPS.Init(device, "LightWarpPS.cso");

		m_linePS.Init(device, "LinePS.cso");
		m_lineVS.Init(device, "LineVS.cso");
		m_lineVS.InitInputLayout(device, lineLayout, ARRAYSIZE(lineLayout));

		dc->VSSetShader(m_3dvs.GetShader(), NULL, 0);
		//dc->PSSetShader(m_3dps.GetShader(), NULL, 0);

		return true;
	}

	bool Renderer::InitScene()
	{
		dc->RSSetState(m_rasterizerState);
		dc->OMSetDepthStencilState(m_dsState, 0);

		std::vector<BBVertex> gridArr;

		// grid
		uint nLines = 11;
		gridArr.resize(nLines * 4);
		for (int i = 0; i < nLines; i++)
		{
			gridArr[2 * i + 0].m_position = float3(i - 5, 0.f, (float)nLines / 2.f);
			gridArr[2 * i + 1].m_position = float3(i - 5, 0.f, -(float)nLines / 2.f);
			gridArr[2 * i + 0].m_color = GRAY_3F;
			gridArr[2 * i + 1].m_color = GRAY_3F;
		}
		for (int i = 0; i < nLines; i++)
		{
			uint index = (nLines * 2) + i * 2;
			gridArr[index + 0].m_position = float3(-(float)nLines / 2.f, 0.f, i - 5);
			gridArr[index + 1].m_position = float3((float)nLines / 2.f, 0.f, i - 5);

			gridArr[index + 0].m_color = GRAY_3F;
			gridArr[index + 1].m_color = GRAY_3F;
		}

		m_grid.CreateVertexBuffer(device, gridArr.data(), gridArr.size());

		dc->OMSetBlendState(m_blendState, NULL, 0xFFFFFFFF);

		//ResourceHandler::AddTexture("missingTexture.png"); // setting missingtexture
		ResourceHandler::AddTexture(".NoTexture.pngg"); // setting missingtexture
		ResourceHandler::AddTexture("ZANormal.png"); // Load LightWarp Texture
		//ResourceHandler::AddTexture("ZATf2esk.png"); // Load LightWarp Texture

		dc->PSSetShaderResources(0, 1, ResourceHandler::GetTexture(1).GetSRVAdress());
		return true;
	}

	void Renderer::InitConstantBuffers()
	{
		m_transformBuffer.CreateConstantBuffer(device, dc);
		dc->VSSetConstantBuffers(0, 1, m_transformBuffer.GetReference());
		m_lightbuffer.CreateConstantBuffer(device, dc);
		dc->PSSetConstantBuffers(0, 1, m_lightbuffer.GetReference());
		m_materialBuffer.CreateConstantBuffer(device, dc);
		dc->PSSetConstantBuffers(1, 1, m_materialBuffer.GetReference());
	}

	void Renderer::ImGuiRender()
	{
		if (m_guiHandler)
		{
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
			ImGuizmo::BeginFrame();
			//m_isHoveringWindow = false;

			m_guiHandler->ImguiRender();

			ImGuTextureDisplay();

			//ImGuiGradientWindow();

			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}
	}

	void Renderer::ImGuTextureDisplay()
	{
		float winvar = 400;
		float offset = 35;
		ImVec2 winSize(winvar, winvar + offset);
		ImGui::SetNextWindowSize(winSize);
		ImGui::Begin("Texture Display", NULL, ImGuiWindowFlags_NoResize);

		ImTextureID tex = m_shadowmap.GetSRV();
		ImGui::Image(tex, { winSize.x, winSize.x });

		ImGui::End();
	}
	using uchar = unsigned char;

	void Renderer::SetLightWarp(const std::string& path)
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

	void Renderer::SetActiveCam(uint idx)
	{
		m_activeCamIndex = idx;
	}

	void RefreshPrefabInstances(std::vector<MeshPrefabRef>& rPrefabs, ID3D11DeviceContext*& dc)
	{
		uint numPrefabsRefs = ComponentHandler::GetNoOfUsedComponents<pt::MeshPrefabRef>();
		uint numPrefabs = ComponentHandler::GetNoOfUsedComponents<pt::MeshPrefabRef>();
		std::vector<Prefab>& prefabsArr = ResourceHandler::GetPrefabArray();
		for (int i = 0; i < numPrefabs; i++)
		{
			prefabsArr[i].GetRefIdx(i);
		}
	}

	void updateInstances(std::vector<Mesh>& meshVec, int numMesher, std::vector<MeshRef>& rMeshrefs, const std::vector<TransformComp>& rTransforms)
	{
		for (int i = 0; i < numMesher; i++)
		{
			uint entId = rMeshrefs[i].EntId();
			if (entId != uint(-1))
			{
				TransformComp ttt = rTransforms[entId];

				rMeshrefs[i].UpdateTransformInstance(meshVec.data(), ttt);
			}
		}
	}

	void drawMeshes(std::vector<MeshRef>& rMeshrefs, std::vector<TransformComp>& rTransforms, std::vector<MeshPrefabRef>& rPrefabs,
		Buffer<hlsl::cbpWorldTransforms3D>& transformBuffer, Buffer<hlsl::cbpMaterialBuffer>* pMAtBuffer, ID3D11DeviceContext*& dc, float deltatime, int& drawCalls)
	{
		uint numMEshRefs = ComponentHandler::GetNoOfUsedComponents<pt::MeshRef>();
		
		if (!pMAtBuffer)
		{
			uint numMEshes = ResourceHandler::GetNoMeshes();
			for (int i = 0; i < numMEshes; i++)
			{
				Mesh* pMesh = ResourceHandler::GetMeshAdress(i);
				uint numInstances = pMesh->GetNrOfUses();
				pMesh->InstancedBind(dc);
				pMesh->MapInstance();

				uint numVerts = pMesh->GetMeshOffsfets()[pMesh->GetNofMeshes()];

				dc->DrawInstanced(numVerts, numInstances, 0, 1);
			}
			/*for (int i = 0; i < numMEshRefs; i++)
			{
				uint entId = rMeshrefs[i].EntId();

				TransformComp* pTransformComp = &rTransforms[entId];
				transformBuffer.Data().world = pTransformComp->GetWorld();
				transformBuffer.MapBuffer();

				Mesh* meshPtr = rMeshrefs[i].GetMeshContainerP();
				meshPtr->Bind(dc);
				meshPtr->MapInstance();


				uint nSubMEshes = meshPtr->GetNofMeshes();

				int v1 = meshPtr->GetMeshOffsfets()[nSubMEshes];
				drawCalls++;
				dc->Draw(v1, 0);
			}*/
		}
		else
		{
			for (int i = 0; i < numMEshRefs; i++)
			{
				uint entId = rMeshrefs[i].EntId();

				TransformComp* pTransformComp = &rTransforms[entId];
				transformBuffer.Data().world = pTransformComp->GetWorld();
				transformBuffer.MapBuffer();

				Mesh* meshPtr = rMeshrefs[i].GetMeshContainerP();
				meshPtr->InstancedBind(dc);

				uint nSubMEshes = meshPtr->GetNofMeshes();

				for (int j = 0; j < nSubMEshes; j++)
				{
					uint matIndex = rMeshrefs[i].GetMaterialIndex(j);
					Material& rMat = ResourceHandler::GetMaterial(matIndex);
					rMat.Set(dc, *pMAtBuffer);
					rMat.UpdateTextureScroll(deltatime);

					int v1 = meshPtr->GetMeshOffsfets()[j + 1], v2 = meshPtr->GetMeshOffsfets()[j];
					drawCalls++;
					// draws first instance in instance buffer, which is identity, because there is already a transform in place
					dc->DrawInstanced(v1 - v2, 1, v2, 0);
					//dc->Draw(v1 - v2, v2);
				}
			}
		}



		//std::vector<Prefab>& prefabsArr = ResourceHandler::GetPrefabArray();
		//uint nOPrefRefs = ComponentHandler::GetNoOfUsedComponents<pt::MeshPrefabRef>();

		//for (int i = 0; i < nOPrefRefs; i++)
		//{
		//	uint entId = rPrefabs[i].EntId();
		//	TransformComp* pTransformComp = &rTransforms[entId];

		//	uint instanceIndex = rPrefabs[i].GetInstIndex(); // instance 0 is identity matrix
		//	uint prefabIndex = rPrefabs[i].GetIndex();

		//	MeshInstance mehsInst;
		//	matrix mat = pTransformComp->GetWorld();

		//	memcpy(&mehsInst.row.x,  &mat[0][0], sizeof(float) * 3);
		//	memcpy(&mehsInst.row1.x, &mat[1][0], sizeof(float) * 3);
		//	memcpy(&mehsInst.row2.x, &mat[2][0], sizeof(float) * 3);
		//	memcpy(&mehsInst.row3.x, &mat[3][0], sizeof(float) * 3);

		//	prefabsArr[prefabIndex].ChangeInstance(instanceIndex, mehsInst);

		//	//rPrefabs[i].RefreshInstance();
		//}
		//uint nOPrefs = prefabsArr.size();
		//transformBuffer.Data().world = matrix(1.f);;
		//transformBuffer.MapBuffer();
		//for (int i = 0; i < nOPrefs; i++)
		//{
		//	if (prefabsArr[i].GetUses() > 0)
		//	{
		//		Mesh* meshPtr = prefabsArr[i].GetMeshPtr();

		//		prefabsArr[i].BindInstanceBuffer(dc);
		//		prefabsArr[i].MapInstBuffer();

		//		meshPtr->Bind(dc);

		//		uint numInstances = prefabsArr[i].GetUses();
		//		uint nSubMEshes = meshPtr->GetNofMeshes();

		//		if (pMAtBuffer)
		//		{
		//			for (int j = 0; j < nSubMEshes; j++)
		//			{
		//				uint matIndex = prefabsArr[i].GetMaterialIndex(j);
		//				Material& rMat = ResourceHandler::GetMaterial(matIndex);
		//				rMat.Set(dc, *pMAtBuffer);
		//				rMat.UpdateTextureScroll(deltatime);

		//				int v1 = meshPtr->GetMeshOffsfets()[j + 1], v2 = meshPtr->GetMeshOffsfets()[j];
		//				drawCalls++;
		//				uint uses = prefabsArr[i].GetUses();
		//				dc->DrawInstanced(v1 - v2, std::min(prefabsArr[i].GetUses(), prefabsArr[i].GetCap()-1), v2, 1);
		//			}
		//		}
		//	}	
		//}
	}

	void Renderer::Render(const float& deltatime)
	{
		float bgColor[] = { .1f,.1f,.1f,1.f };

		dc->ClearRenderTargetView(m_rtv, bgColor);
		dc->ClearDepthStencilView(m_dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
		dc->OMSetBlendState(m_blendState, NULL, 0xFFFFFFFF);

		dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		dc->RSSetState(m_rasterizerState);

		std::vector<MeshRef>& rMeshrefs = ComponentHandler::GetComponentArray<MeshRef>();
		std::vector<MeshPrefabRef>& rPrefabrefs = ComponentHandler::GetComponentArray<MeshPrefabRef>();
		std::vector<TransformComp>& rTransforms = ComponentHandler::GetComponentArray<TransformComp>();

		// -------------------------------------- Update transforms with rigidbodies ---------------------------------------------------
		std::vector<PhysicsBody>& rRigidBodies = ComponentHandler::GetComponentArray<PhysicsBody>();
		uint numRigidBodies = ComponentHandler::GetNoOfUsedComponents<PhysicsBody>();
		for (int i = 0; i < numRigidBodies; i++)
		{
			uint entId = rRigidBodies[i].EntId();
			rRigidBodies[i].UpdateTransform(rTransforms[entId]);
		}


		dc->VSSetShader(m_3dvs.GetShader(), NULL, 0);
		dc->PSSetShader(NULL, NULL, 0);
		dc->IASetInputLayout(m_3dvs.GetInputLayout());

		Camera& scam = ComponentHandler::GetComponentByIndex<Camera>(m_shadowCamIndex);

		m_lightbuffer.Data().cbShadowBias = im->shadowBias;
		//m_lightbuffer.Data().pointLightDistance = 10.f;
		m_lightbuffer.Data().shadowDir = scam.GetForwardV();

		// -------------------------------------- Lights ---------------------------------------------------

		std::vector<pt::Light>& r_lights = ComponentHandler::GetComponentArray<pt::Light>();
		uint numLights = ComponentHandler::GetNoOfUsedComponents<pt::Light>();

		m_lightbuffer.Data().numLights = numLights;

		for (int i = 0; i < numLights; i++)
		{
			m_multiLightBuffer.Data(i).clr = r_lights[i].GetLightData().clr;
			m_multiLightBuffer.Data(i).pos = r_lights[i].GetLightData().pos;
			m_multiLightBuffer.Data(i).dire = r_lights[i].GetLightData().dire;
		}
		m_multiLightBuffer.MapBuffer();

		
		{
			int noCams = ComponentHandler::GetNoOfUsedComponents<Camera>();
			pt::TransformComp& camTransform = ComponentHandler::GetComponentByIndex<pt::TransformComp>(m_activeCamIndex);
			m_lightbuffer.Data().camPos = camTransform.GetPosition();
			m_lightbuffer.MapBuffer();

			m_transformBuffer.Data().viewProj = scam.GetProjMatrix() * scam.GetViewMatrix();
			m_transformBuffer.Data().lightViewProj = scam.GetProjMatrix() * scam.GetViewMatrix();
			//m_transformBuffer.Data().world = matrix(1.f);
			m_transformBuffer.MapBuffer();
			m_shadowmap.Bind(dc, 10);
		}
		

		uint numMEshRefs = (uint)rMeshrefs.size();
		uint offset = 0;

		updateInstances(ResourceHandler::GetMeshArrayReference(), rMeshrefs.size(), rMeshrefs, rTransforms);

		drawMeshes(rMeshrefs, rTransforms, rPrefabrefs, m_transformBuffer, NULL, dc, deltatime, im->m_drawCalls);

		// --------------------------End of shadw pass-----------------------------------------------

		pt::Camera& cc = ComponentHandler::GetComponentByIndex<pt::Camera>(m_activeCamIndex);
		dc->OMSetRenderTargets(1, &m_rtv, m_dsView);
		dc->RSSetViewports(1, &m_viewport);

		m_transformBuffer.Data().viewProj = cc.GetProjMatrix() * cc.GetViewMatrix();
		m_transformBuffer.MapBuffer();

		dc->IASetInputLayout(m_3dvs.GetInputLayout());
		dc->VSSetShader(m_3dvs.GetShader(), NULL, 0);
		dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		dc->PSSetShader(m_toonPS.GetShader(), NULL, 0);

		m_materialBuffer.Data().flags = 0;
		m_shadowmap.BindSRV(dc, 10);
		// iterate through meshrefs
		m_transformBuffer.MapBuffer();
		drawMeshes(rMeshrefs, rTransforms, rPrefabrefs, m_transformBuffer, &m_materialBuffer, dc, deltatime, im->m_drawCalls);

		m_transformBuffer.Data().world = matrix(1.f);
		dc->VSSetShader(m_lineVS.GetShader(), NULL, 0);
		dc->IASetInputLayout(m_lineVS.GetInputLayout());
		dc->PSSetShader(m_linePS.GetShader(), NULL, 0);
		dc->RSSetState(m_wireFrameState);

		// --------------------------- physics visuals ---------------------------

#ifdef PHYSDEBUG
		if (mp_debufrenderer && mp_debufrenderer->GetNoPhysBodies())
			mp_debufrenderer->DrawBuffers(dc);
#endif // PHYSDEBUG

		// --------------------------- Draw grid ---------------------------


		if (im->m_drawGrid)
		{
			m_transformBuffer.Data().world = matrix(1);
			m_transformBuffer.MapBuffer();
			dc->IASetVertexBuffers(0, 1, m_grid.GetReference(), m_grid.GetStrideP(), &offset);
			dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
			dc->Draw(m_grid.GetBufferSize(), 0);
		}


		ImGuiRender();
		im->m_drawCalls = 0;
		m_swapChain->Present((UINT)im->useVsync, NULL);
	}

	ID3D11Device* Renderer::GetDevice() const
	{
		return device;
	}

	ID3D11DeviceContext* Renderer::GetDeviceContext() const
	{
		return dc;
	}

	void Renderer::ShutDown()
	{
		DestroyWindow(*m_pHWND);
	}
}