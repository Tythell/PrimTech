#pragma once
#include <d3d11.h>
#include <Windows.h>
#include "Utility/Popup.h"
#include "DXTK/SimpleMath.h"

namespace sm = DirectX::SimpleMath;
using Vector2i = DirectX::XMINT2;

class Window;

class DX11Addon
{
public:
	DX11Addon(Window& window);
	~DX11Addon();


private:
	bool initSwapChain();
	bool initRTV();
	bool SetupDSAndVP();
	bool InitRastNSampState();

	const unsigned int m_width, m_height;

	HWND* m_pHWND = nullptr;

	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_dc = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;

	ID3D11RenderTargetView* m_rtv = nullptr;
	ID3D11Texture2D* m_depthStencilBuffer = nullptr;
	ID3D11DepthStencilView* m_dsView = nullptr;
	ID3D11DepthStencilState* m_dsState;

	D3D11_VIEWPORT m_viewport;

	ID3D11RasterizerState* m_rasterizerState = nullptr;
	ID3D11SamplerState* m_sampState = nullptr;


	Window* m_pWin = nullptr;
};

