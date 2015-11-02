// test.cpp : ����Ӧ�ó������ڵ㡣
//
#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�: 
#include <windows.h>
#include <Commdlg.h>
#include <Windowsx.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "PlatformDelegate.h"
#include "WinDesk\Peach3DPlatformWinDesk.h"

#define MAX_LOADSTRING 100

// ȫ�ֱ���: 
HINSTANCE hInst;										// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING] = L"meshviewer";			// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING] = L"Peach3DMeshViewerWindow";	// ����������
Peach3D::PlatformWinDesk    gPlatform;                  // peach3d platform
PlatformDelegate	            gDelegate;              // app delegate

// �˴���ģ���а����ĺ�����ǰ������: 
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

 	// TODO:  �ڴ˷��ô��롣
	MSG msg;

	MyRegisterClass(hInstance);
	// ִ��Ӧ�ó����ʼ��: 
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	// ����Ϣѭ��: 
	while (!gPlatform.isTerminating())
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// ��Ⱦÿһ֡��renderOneFrame���ж��Ƿ�ִ��
		gPlatform.renderOneFrame(0.0f);
	}

	return (int) msg.wParam;
}



//
//  ����:  MyRegisterClass()
//
//  Ŀ��:  ע�ᴰ���ࡣ
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
//   ����:  InitInstance(HINSTANCE, int)
//
//   Ŀ��:  ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   // ָ�����ڵĴ�С
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

   // ��ʼ��ƽ̨
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
//  ����:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
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
		// �˳�platform
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
