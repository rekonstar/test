#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include "menu.h"
#include <d3d11.h>
#include <dwmapi.h>
#include "../../world/actor_loops/visual.h"
#include "../../utils/protection/ex.h"

static HWND Window = NULL;
ID3D11Device* g_pd3dDevice;
ID3D11DeviceContext* g_pd3dDeviceContext;
IDXGISwapChain* g_pSwapChain;
ID3D11RenderTargetView* g_mainRenderTargetView;
HWND hwnd = NULL;
MSG Message = { NULL };

__forceinline void CreateRenderTarget ( )
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer ( 0, IID_PPV_ARGS ( &pBackBuffer ) );
	g_pd3dDevice->CreateRenderTargetView ( pBackBuffer, NULL, &g_mainRenderTargetView );
	pBackBuffer->Release ( );
}
__forceinline void CleanupRenderTarget ( )
{
	if ( g_mainRenderTargetView ) { g_mainRenderTargetView->Release ( ); g_mainRenderTargetView = NULL; }
}
__forceinline void CleanupDeviceD3D ( )
{
	CleanupRenderTarget ( );
	if ( g_pSwapChain ) { g_pSwapChain->Release ( ); g_pSwapChain = NULL; }
	if ( g_pd3dDeviceContext ) { g_pd3dDeviceContext->Release ( ); g_pd3dDeviceContext = NULL; }
	if ( g_pd3dDevice ) { g_pd3dDevice->Release ( ); g_pd3dDevice = NULL; }
}


__forceinline auto MainRender ( ) -> void
{
	ImGui_ImplDX11_NewFrame ( );
	ImGui_ImplWin32_NewFrame ( );
	ImGui::NewFrame ( );

	if ( GetAsyncKeyState ( VK_INSERT ) & 1 ) {
		settings::menu = !settings::menu;
	}

	actor_loop ( );
	if ( settings::menu ) {
		menu::menu ( );
	}


	ImGui::EndFrame ( );
	ImGui::Render ( );
	ImVec4 clear_color = ImVec4 ( 0., 0., 0., 0. );
	g_pd3dDeviceContext->OMSetRenderTargets ( 1, &g_mainRenderTargetView, NULL );
	g_pd3dDeviceContext->ClearRenderTargetView ( g_mainRenderTargetView, ( float* ) &clear_color );
	ImGui_ImplDX11_RenderDrawData ( ImGui::GetDrawData ( ) );
	g_pSwapChain->Present ( 0, 0 );
}


namespace imguisetup {


	__forceinline auto Overlay ( ) -> void
	{
		while ( !Window ) {
			Window = FindWindowA ( skCrypt ( "MedalOverlayClass" ).decrypt ( ), skCrypt ( "MedalOverlay" ).decrypt ( ) );
			Sleep ( 10 );
		}

		MARGINS Margin = { -1 };
		DwmExtendFrameIntoClientArea ( Window, &Margin );
		SetWindowPos ( Window, 0, 0, 0, GetSystemMetrics ( SM_CXSCREEN ), GetSystemMetrics ( SM_CYSCREEN ), SWP_SHOWWINDOW );
		ShowWindow ( Window, SW_SHOW );
		UpdateWindow ( Window );
	}

	__forceinline auto SetupImgui ( ) -> HRESULT
	{
		DXGI_SWAP_CHAIN_DESC SwapChainDesc;
		ZeroMemory ( &SwapChainDesc, sizeof ( SwapChainDesc ) );
		SwapChainDesc.Windowed = TRUE;
		SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		SwapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		SwapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
		SwapChainDesc.BufferDesc.Height = 0;
		SwapChainDesc.BufferDesc.Width = 0;
		SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapChainDesc.BufferCount = 2;
		SwapChainDesc.OutputWindow = Window;
		SwapChainDesc.SampleDesc.Count = 8; // 8
		SwapChainDesc.SampleDesc.Quality = 0;
		UINT createDeviceFlags = 0;
		D3D_FEATURE_LEVEL featureLevel;
		const D3D_FEATURE_LEVEL featureLevelArray [ 2 ] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
		if ( D3D11CreateDeviceAndSwapChain ( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &SwapChainDesc, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext ) != S_OK )
			return false;

		IMGUI_CHECKVERSION ( );
		ImGui::CreateContext ( );
		ImGuiIO& io = ImGui::GetIO ( ); ( void ) io;

		ImGuiStyle& style = ImGui::GetStyle ( );

		ImFontConfig cfg;


		ID3D11Texture2D* pBackBuffer;
		g_pSwapChain->GetBuffer ( 0, IID_PPV_ARGS ( &pBackBuffer ) );
		g_pd3dDevice->CreateRenderTargetView ( pBackBuffer, NULL, &g_mainRenderTargetView );

		ImGui_ImplWin32_Init ( Window );
		ImGui_ImplDX11_Init ( g_pd3dDevice, g_pd3dDeviceContext );

		pBackBuffer->Release ( );
	}

	__forceinline auto GuiLoop ( ) -> void
	{
		static RECT old_rc;
		ZeroMemory ( &Message, sizeof ( MSG ) );

		while ( Message.message != WM_QUIT )
		{
			if ( PeekMessageA ( &Message, Window, 0, 0, PM_REMOVE ) )
			{
				TranslateMessage ( &Message );
				DispatchMessageA ( &Message );
			}

			ImGuiIO& io = ImGui::GetIO ( );
			POINT p;
			POINT xy;

			GetCursorPos ( &p );
			long styleProc = GetWindowLongA ( Window, GWL_EXSTYLE );
			io.MousePos.x = p.x;
			io.MousePos.y = p.y;
			io.DeltaTime = 1.0f / 60.0f;
			io.ImeWindowHandle = Window;

			if ( GetAsyncKeyState ( VK_LBUTTON ) ) {
				io.MouseDown [ 0 ] = true;
				io.MouseClicked [ 0 ] = true;
				io.MouseClickedPos [ 0 ].x = io.MousePos.x;
				io.MouseClickedPos [ 0 ].x = io.MousePos.y;
			}
			else
				io.MouseDown [ 0 ] = false;


			MainRender ( );
		}
		ImGui_ImplDX11_Shutdown ( );
		ImGui_ImplWin32_Shutdown ( );
		ImGui::DestroyContext ( );

		CleanupDeviceD3D ( );
	}
}