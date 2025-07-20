#include "gui.h"

static LPDIRECT3D9              g_pD3D = nullptr;
static LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
static bool                     g_DeviceLost = false;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};



bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = nullptr; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

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
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

static void AttachGameWindow(HWND GameHwnd, HWND MyHwnd)
{
    RECT GameRect{};
    POINT GamePoint{};
    GetClientRect(GameHwnd, &GameRect);
    ClientToScreen(GameHwnd, &GamePoint);
    ImVec2 Size, Pos;
    Pos = ImVec2(static_cast<float>(GamePoint.x), static_cast<float>(GamePoint.y));
    Size = ImVec2(static_cast<float>(GameRect.right), static_cast<float>(GameRect.bottom));

    SetWindowPos(MyHwnd, HWND_TOP, static_cast<int>(Pos.x), static_cast<int>(Pos.y), static_cast<int>(Size.x), static_cast<int>(Size.y), SWP_SHOWWINDOW);
}



int MainThread() {

	HWND GameHwnd = FindWindowA("MainWindow", "植物大战僵尸中文版");

    
    ImGui_ImplWin32_EnableDpiAwareness();
    float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

    // Create application window
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST,
        wc.lpszClassName, L"Dear ImGui DirectX9 Example",
        WS_POPUP, 100, 100, (int)(1280 * main_scale), (int)(800 * main_scale), nullptr, nullptr, wc.hInstance, nullptr);
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)
    
    style.Colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f); // 文本：近纯白
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f); // 禁用文本：中灰
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f); // 窗口背景：高级炭黑
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.12f, 0.14f, 0.50f); // 子窗口背景：稍亮炭黑
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.12f, 0.14f, 0.95f); // 弹出窗口：稍亮炭黑
    style.Colors[ImGuiCol_Border] = ImVec4(0.30f, 0.30f, 0.32f, 0.50f); // 边框：深灰
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f); // 边框阴影：无
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.17f, 0.50f); // 控件背景：深炭黑
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.27f, 0.75f); // 控件悬停：稍亮灰
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.35f, 0.35f, 0.37f, 1.00f); // 控件激活：中灰
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f); // 标题栏：高级炭黑
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f); // 标题栏激活：稍亮炭黑
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.12f, 0.75f); // 标题栏折叠：高级炭黑
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.12f, 0.14f, 1.00f); // 菜单栏：稍亮炭黑
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.12f, 0.30f); // 滚动条背景：透明炭黑
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.30f, 0.32f, 1.00f); // 滚动条抓手：深灰
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.42f, 1.00f); // 滚动条悬停：中灰
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.50f, 0.50f, 0.52f, 1.00f); // 滚动条激活：稍亮灰
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f); // 勾选标记：近纯白
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f); // 滑块抓手：近纯白
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f); // 滑块激活：纯白
    style.Colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f); // 按钮：稍亮炭黑
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.30f, 0.32f, 1.00f); // 按钮悬停：深灰
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.40f, 0.40f, 0.42f, 1.00f); // 按钮激活：中灰
    style.Colors[ImGuiCol_Header] = ImVec4(0.25f, 0.25f, 0.27f, 0.80f); // 表头：稍亮灰
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.35f, 0.35f, 0.37f, 0.90f); // 表头悬停：中灰
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.45f, 0.45f, 0.47f, 1.00f); // 表头激活：稍亮灰
    style.Colors[ImGuiCol_Separator] = ImVec4(0.30f, 0.30f, 0.32f, 0.50f); // 分隔线：深灰
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.40f, 0.40f, 0.42f, 0.78f); // 分隔线悬停：中灰
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.50f, 0.50f, 0.52f, 1.00f); // 分隔线激活：稍亮灰
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.90f, 0.90f, 0.90f, 0.25f); // 调整手柄：透明近白
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.90f, 0.90f, 0.90f, 0.67f); // 调整手柄悬停：近白
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.95f); // 调整手柄激活：纯白
    style.Colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.15f, 0.17f, 0.86f); // 标签：深炭黑
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.30f, 0.30f, 0.32f, 0.90f); // 标签悬停：深灰
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.25f, 0.25f, 0.27f, 1.00f); // 标签激活：稍亮灰
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.12f, 0.12f, 0.14f, 0.98f); // 标签未聚焦：稍亮炭黑
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f); // 标签未聚焦激活：稍亮炭黑
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f); // 折线图：近纯白
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f); // 折线图悬停：纯白
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f); // 直方图：近纯白
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f); // 直方图悬停：纯白
    style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f); // 表格头：稍亮炭黑
    style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.30f, 0.30f, 0.32f, 1.00f); // 表格强边框：深灰
    style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.30f, 0.30f, 0.32f, 0.50f); // 表格轻边框：透明深灰
    style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f); // 表格行背景：透明
    style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f); // 表格交替行：微白
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.40f, 0.40f, 0.42f, 0.35f); // 文本选择背景：中灰
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.40f, 0.40f, 0.42f, 0.90f); // 拖放目标：中灰
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.90f, 0.90f, 0.90f, 0.90f); // 导航高亮：近纯白
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.90f, 0.90f, 0.90f, 0.70f); // 窗口导航高亮：透明近白
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.20f, 0.20f, 0.22f, 0.20f); // 窗口导航背景：透明炭黑
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.22f, 0.35f); // 模态窗口背景：透明炭黑

    // 调整圆角和间距，增强简约高级感
    style.WindowRounding = 6.0f;  // 窗口圆角：适中
    style.FrameRounding = 3.0f;  // 控件圆角：轻微
    style.PopupRounding = 3.0f;  // 弹出窗口圆角：轻微
    style.ScrollbarRounding = 3.0f;  // 滚动条圆角：轻微
    style.GrabRounding = 3.0f;  // 抓手圆角：轻微
    style.TabRounding = 3.0f;  // 标签圆角：轻微
    style.WindowPadding = ImVec2(10.0f, 10.0f); // 窗口内边距：稍大
    style.FramePadding = ImVec2(5.0f, 4.0f);  // 控件内边距：适中
    style.ItemSpacing = ImVec2(10.0f, 5.0f); // 元素间距：宽松
    style.ItemInnerSpacing = ImVec2(5.0f, 5.0f);  // 元素内部间距：适中
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f); // 窗口标题对齐：居中



    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);


    ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msyh.ttc");

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0, 0, 0, 0);

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

        // Handle lost D3D9 device
        if (g_DeviceLost)
        {
            HRESULT hr = g_pd3dDevice->TestCooperativeLevel();
            if (hr == D3DERR_DEVICELOST)
            {
                ::Sleep(10);
                continue;
            }
            if (hr == D3DERR_DEVICENOTRESET)
                ResetDevice();
            g_DeviceLost = false;
        }

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            g_d3dpp.BackBufferWidth = g_ResizeWidth;
            g_d3dpp.BackBufferHeight = g_ResizeHeight;
            g_ResizeWidth = g_ResizeHeight = 0;
            ResetDevice();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();



        MenuInit();




        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
        g_pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);
        if (result == D3DERR_DEVICELOST)
            g_DeviceLost = true;
        AttachGameWindow(GameHwnd, hwnd);
    }

    // Cleanup
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;

}