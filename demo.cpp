#pragma once

#include "Imgui\\imconfig.h"
#include "Imgui\\imgui.h"
#include "Imgui\\imgui_impl_dx11.h"
#include "Imgui\\imgui_impl_win32.h"
#include "Imgui\\imgui_internal.h"
#include "Imgui\\imstb_rectpack.h"
#include "Imgui\\imstb_textedit.h"
#include "Imgui\\imstb_truetype.h"
#include "font.h"
#include <time.h>
#include <string.h>
#include <iostream>
#include <tchar.h>
#include <d3d11.h>
#include "GameCheat.h"
#pragma comment(lib,"d3d11.lib")
#pragma execution_character_set("utf-8")  //设置编码格式可以正常显示中文
using namespace std;


static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

DWORD GetProcessIdByName(const std::wstring& processName);
DWORD PID;
HANDLE hprocess;
JX2Role role;


int main()
{
	// Create application window
   //ImGui_ImplWin32_EnableDpiAwareness();
	WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
	::RegisterClassExW(&wc);
	HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX11 Example", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClassW(wc.lpszClassName, wc.hInstance);
		return 1;
	}

	// Show the window
	::ShowWindow(hwnd, SW_HIDE);
	::UpdateWindow(hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;
	//io.ConfigViewportsNoDefaultParent = true;
	//io.ConfigDockingAlwaysTabBar = true;
	//io.ConfigDockingTransparentPayload = true;
	//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
	//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}


	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msyh.ttc", 18.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msyh.ttc", 18.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
	//IM_ASSERT(font != nullptr);

	// Our state
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Main loop
	bool done = false;
	while (!done)
	{
		// Poll and handle messages (inputs, window resize, etc.)
		// See the WndProc() function below for our to dispatch events to the Win32 backend.
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				done = true;
		}
		if (done)
			break;

		// Handle window resize (we don't resize directly in the WM_SIZE handler)
		if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
		{
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
			g_ResizeWidth = g_ResizeHeight = 0;
			CreateRenderTarget();
		}

		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
      
		ImGuiStyle& Style = ImGui::GetStyle();
		auto Color = Style.Colors;
		Style.ChildRounding = 8.0f;
		Style.FrameRounding = 5.0f;
		auto c= (float*)&style.WindowTitleAlign;
		c[0] = 0.5f;

		Color[ImGuiCol_Button] = ImColor(51, 120, 255, 255);
		Color[ImGuiCol_ButtonHovered] = ImColor(71, 140, 255, 255);
		Color[ImGuiCol_ButtonActive] = ImColor(31, 100, 225, 255);

		Color[ImGuiCol_FrameBg] = ImColor(54, 54, 54, 150);
		Color[ImGuiCol_FrameBgActive] = ImColor(42, 42, 42, 150);
		Color[ImGuiCol_FrameBgHovered] = ImColor(100, 100, 100, 150);

		Color[ImGuiCol_CheckMark] = ImColor(51, 120, 255, 255);

		Color[ImGuiCol_SliderGrab] = ImColor(51, 120, 255, 255);
		Color[ImGuiCol_SliderGrabActive] = ImColor(31, 100, 225, 255);

		Color[ImGuiCol_Header] = ImColor(51, 120, 255, 255);
		Color[ImGuiCol_HeaderHovered] = ImColor(71, 140, 255, 255);
		Color[ImGuiCol_HeaderActive] = ImColor(31, 100, 225, 255);
		//设置颜色
		static bool checkbox1 = false, checkbox2=0, checkbox3=0, checkbox4 = 0;
	
		static int Tab = 0;
		enum Tab
		{
			button1,
			button2,

		};
		static bool show = true;
		if (GetAsyncKeyState(VK_INSERT) & 1) show = !show;
		if(show)
		{
		ImGui::Begin(u8"xrz辅助", &show, ImGuiWindowFlags_NoSavedSettings + ImGuiWindowFlags_NoResize+ ImGuiWindowFlags_NoScrollbar);//开始绘制窗口 //设置窗口样式
		//ImGui::GetWindowDrawList()->AddLine({ ImGui::GetWindowPos().x+100,ImGui::GetWindowPos().y + 50 }, { ImGui::GetWindowPos().x + 100,ImGui::GetWindowPos().y + 40 }, ImColor(100, 100, 100, 255));
		ImGui::SetWindowSize({ 600.0f,400.0f });//设置窗口大小 长 宽/1.5
		
		ImGui::SetCursorPos({ 37.5f,32.0f });//设置下一个窗口的位置
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(2 / 7.0f, 0.6f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(2 / 7.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(2 / 7.0f, 0.8f, 0.8f));
		if (ImGui::Button("变态功能", { 75.f,50.f }))
		{
			Tab = Tab::button1;
		}
		ImGui::PopStyleColor(3);

		ImGui::SetCursorPos({ 137.5f,32.0f });//设置下一个窗口的位置
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(3 / 7.0f, 0.6f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(3 / 7.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(3 / 7.0f, 0.8f, 0.8f));
		if (ImGui::Button("绘制功能", { 75.f,50.f }))
		{
			Tab = Tab::button2;
		}
		ImGui::PopStyleColor(3);

		ImGui::SetCursorPos({ 237.5f,32.0f });//设置下一个窗口的位置
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(5 / 7.0f, 0.6f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(5 / 7.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(5 / 7.0f, 0.8f, 0.8f));
		if (ImGui::Button("退出辅助",{75,50}))
			PostQuitMessage(0);
		ImGui::PopStyleColor(3);

		ImGui::SetCursorPos({ .0f,90.0f });
		ImGui::BeginChild(u8"Fucking", { 600.0f,310.0f }, true);
		switch (Tab)
		{
			//ImGuiInputFlags_
		case Tab::button1:
			//功能区
			if (ImGui::Button("获取PID"))
			{
				std::wstring processName = L"sword2_720p_UI.exe";
				 PID = GetProcessIdByName(processName);
			};
			ImGui::SameLine();
			ImGui::InputInt(u8"PID", (int*)&PID);
			if (ImGui::Checkbox("血量不减",&checkbox1))
			{
				hprocess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
				if (hprocess)
				{
					LPVOID lCode = VirtualAllocEx(hprocess, NULL, 1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

					char data[]{ 0x81,0xfe,  //cmp 汇编
					  0x00,0x00,0x00,0x00 ,//人物地址
					0x75,0x02 ,   //jne
					0x31,0xff,   //xor
						0x8b,0xd0,  //mov
						0x29,0xfa,   //sub
						0x39,0xca,    //cmp
						0xe9 ,0x00,0x00,0x00,0x00  //jmp
					};

					int jmpCodeReturn = 0x41fd67 - ((int)lCode + 0x10) - 0x5;
					int* nCode = (int*)(data + 0x11);
					nCode[0] = jmpCodeReturn;
					nCode = (int*)(data + 0x2);
					nCode[0] = 0x4be180;

					WriteProcessMemory(hprocess, lCode, data, 0x21, NULL);

					char E9code[] = { 0xe9,0x00,0x00,0x00,0x00 ,0x90 };
					jmpCodeReturn = (int)lCode - 0x41fd61 - 0x5;
					nCode = (int*)(E9code + 1);
					nCode[0] = jmpCodeReturn;
					DWORD dpro = 0;
					VirtualProtectEx(hprocess, (LPVOID)0x41fd61, 6, PAGE_EXECUTE_READWRITE, &dpro);
					WriteProcessMemory(hprocess, (LPVOID)0x41fd61, E9code, 0x6, NULL);


				}
				else
				{
					checkbox1 = false;
					cout << "memory error\n";
				}
			}; 
			if (ImGui::Checkbox("变态属性", &checkbox2))
			{
				hprocess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
				if (hprocess)
				{
					
						LPVOID prole = (LPVOID)0x4be180;
						ReadProcessMemory(hprocess, prole, &role, sizeof(JX2Role), NULL);
						role.mp[0] = role.mp[1]=role.tp[0]=role.tp[1]=role.hp[0]=role.hp[1] = 999999;
						WriteProcessMemory(hprocess, prole, &role, sizeof(JX2Role), NULL);
					
				}
				else {
					cout << "error2\n";
					checkbox2 = false;
				}


			};
			if (ImGui::Checkbox("一击必杀", &checkbox3))
			{
				hprocess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
				if (hprocess)
				{
					LPVOID prole = (LPVOID)0x4be180;
					ReadProcessMemory(hprocess, prole, &role, sizeof(JX2Role), NULL);
					role.shuxing[0] = role.shuxing[1] = role.shuxing[2] = 999999;
					WriteProcessMemory(hprocess, prole, &role, sizeof(JX2Role), NULL);





				}
				else {
					cout << "error3\n";
					checkbox3 = false;
				}


			};
			if (ImGui::Checkbox("鼠标瞬移", &checkbox4))
			{
				hprocess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
				if (hprocess)
				{
					




				}
				else {
					cout << "error4\n";
					checkbox4 = false;
				}
			};


			break;
	
		case Tab::button2:			
			ImGui::Button(u8"人物方框 按钮测试");
			ImGui::Button(u8"物品方框 按钮测试");
			break;
	
		}

		ImGui::EndChild();
		ImGui::End(); 
		}
		






		

		// Rendering
		ImGui::Render();
		const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		g_pSwapChain->Present(1, 0); // Present with vsync
		//g_pSwapChain->Present(0, 0); // Present without vsync
	}

	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClassW(wc.lpszClassName, wc.hInstance);

	return 0;
	
}

bool CreateDeviceD3D(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
		res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0 // From Windows SDK 8.1+ headers
#endif

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{

	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
			return 0;
		g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
		g_ResizeHeight = (UINT)HIWORD(lParam);
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	case WM_DPICHANGED:
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
		{
			//const int dpi = HIWORD(wParam);
			//printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
			const RECT* suggested_rect = (RECT*)lParam;
			::SetWindowPos(hWnd, nullptr, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
		}
		break;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}


DWORD GetProcessIdByName(const std::wstring& processName) {
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE) {
		return 0;
	}

	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(snapshot, &entry)) {
		CloseHandle(snapshot);
		return 0;
	}

	DWORD processId = 0;
	do {
		std::wstring currentProcessName = entry.szExeFile;
		if (currentProcessName == processName) {
			processId = entry.th32ProcessID;
			break;
		}
	} while (Process32Next(snapshot, &entry));

	CloseHandle(snapshot);

	return processId;
}