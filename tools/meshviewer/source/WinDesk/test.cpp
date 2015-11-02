// test.cpp : 定义应用程序的入口点。
//
#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件: 
#include <windows.h>
#include <Commdlg.h>
#include <Windowsx.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "PlatformDelegate.h"
#include "WinDesk\Peach3DPlatformWinDesk.h"

#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;										// 当前实例
TCHAR szTitle[MAX_LOADSTRING] = L"meshviewer";			// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING] = L"Peach3DMeshViewerWindow";	// 主窗口类名
Peach3D::PlatformWinDesk    gPlatform;                  // peach3d platform
PlatformDelegate	            gDelegate;              // app delegate

// 此代码模块中包含的函数的前向声明: 
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO:  在此放置代码。
	MSG msg;

	MyRegisterClass(hInstance);
	// 执行应用程序初始化: 
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	// 主消息循环: 
	while (!gPlatform.isTerminating())
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// 渲染每一帧，renderOneFrame中判断是否执行
		gPlatform.renderOneFrame(0.0f);
	}

	return (int) msg.wParam;
}



//
//  函数:  MyRegisterClass()
//
//  目的:  注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, L"test.ico");
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, L"small.ico");

	return RegisterClassEx(&wcex);
}

//
//   函数:  InitInstance(HINSTANCE, int)
//
//   目的:  保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   // 指定窗口的大小
   RECT rect;
   rect.left = 0;
   rect.top = 0;
   rect.right = 800;
   rect.bottom = 600;
   AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
       CW_USEDEFAULT, 0, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   // 初始化平台
   Peach3D::PlatformCreationParams   params;
   params.delegate = &gDelegate;
   params.MSAA = 0;
   params.width = 800;
   params.height = 600;
   params.window = (void*)hWnd;
   //params.maxFramsePerSecond = 30.0f;
   gPlatform.initWithParams(params);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void triggerEvent(Peach3D::ClickEvent type, LPARAM lParam)
{
    const float screenHeight = (float)gPlatform.getCreationParams().height;
    std::vector<uint> clickIds = { 1 };
    std::vector<Peach3D::Vector2> poss;
    if (type == Peach3D::ClickEvent::eScrollWheel) {
        float delta = GET_WHEEL_DELTA_WPARAM(lParam);
        delta = (delta > 1.0f || delta < -1.0f) ? delta / (float)WHEEL_DELTA : delta * (float)WHEEL_DELTA;
        poss.push_back(Peach3D::Vector2(0, delta));
    }
    else {
        poss.push_back(Peach3D::Vector2((float)GET_X_LPARAM(lParam), screenHeight - (float)GET_Y_LPARAM(lParam)));
    }
    gPlatform.getEventDispatcher()->triggerClickEvent(type, clickIds, poss);
}

//
//  函数:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		break;
    case WM_KEYDOWN:
        // The virtual-key code of the nonsystem key : wParam 
        gPlatform.getEventDispatcher()->triggerKeyboardEvent(Peach3D::KeyboardEvent::eKeyDown, Peach3D::KeyCode::eUnknow);
        break;
    case WM_KEYUP:
        gPlatform.getEventDispatcher()->triggerKeyboardEvent(Peach3D::KeyboardEvent::eKeyUp, Peach3D::KeyCode::eUnknow);
        break;
    case WM_MOUSEMOVE:
        if (GetAsyncKeyState(VK_LBUTTON)) {
            triggerEvent(Peach3D::ClickEvent::eDrag, lParam);
        }
        else if (GetAsyncKeyState(VK_RBUTTON)) {
            triggerEvent(Peach3D::ClickEvent::eRightDrag, lParam);
        }
        else {
            triggerEvent(Peach3D::ClickEvent::eMoved, lParam);
        }
        break;
    case WM_LBUTTONDOWN:
        triggerEvent(Peach3D::ClickEvent::eDown, lParam);
        break;
    case WM_LBUTTONUP:
        triggerEvent(Peach3D::ClickEvent::eUp, lParam);
        break;
    case WM_RBUTTONDOWN:
        triggerEvent(Peach3D::ClickEvent::eRightDown, lParam);
        break;
    case WM_RBUTTONUP:
        triggerEvent(Peach3D::ClickEvent::eRightUp, lParam);
        break;
    case WM_MOUSEWHEEL:
        triggerEvent(Peach3D::ClickEvent::eScrollWheel, wParam);
        break;
	case WM_DESTROY:
		PostQuitMessage(0);
		// 退出platform
		gPlatform.terminate();
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// open choose file dlg
void openChooseFilePannel(const std::vector<std::string>& extList, std::function<void(const char* file)> chooseCallFunc)
{
    TCHAR szPathName[MAX_PATH];
    lstrcpy(szPathName, TEXT(""));
    OPENFILENAME ofn = { sizeof(OPENFILENAME) };
    ofn.hwndOwner = GetForegroundWindow();
    ofn.lpstrFilter = L"(*.obj)\0*.obj\0\0";
    // set buffer to receive path
    ofn.lpstrFile = szPathName;
    ofn.nMaxFile = sizeof(szPathName);
    ofn.lpstrTitle = TEXT("choose model file");
    // set start file dir
    TCHAR szCurDir[MAX_PATH];
    GetCurrentDirectory(sizeof(szCurDir), szCurDir);
    ofn.lpstrInitialDir = szCurDir;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
    BOOL bOk = GetOpenFileName(&ofn);
    if (bOk)
    {
        int length = WideCharToMultiByte(CP_UTF8, 0, szPathName, -1, nullptr, 0, nullptr, nullptr);
        char* utf8String = (char*)malloc(length + 1);
        WideCharToMultiByte(CP_UTF8, 0, szPathName, -1, utf8String, length + 1, nullptr, nullptr);
        chooseCallFunc(utf8String);
        free(utf8String);
    }
}
