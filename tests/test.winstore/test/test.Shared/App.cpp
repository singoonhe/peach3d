#include "App.h"
#include "Peach3D.h"
#include "PlatformDelegate.h"

#include <ppltasks.h>
using namespace Test;
using namespace Peach3D;

using namespace concurrency;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
#if (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
using namespace Windows::Phone::UI::Input;
#endif

// 应用代理类
PlatformDelegate gAppDelegate;
// 主函数仅用于初始化我们的 IFrameworkView 类。
[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
    auto direct3DApplicationSource = ref new Direct3DApplicationSource();
    CoreApplication::Run(direct3DApplicationSource);
    return 0;
}

IFrameworkView^ Direct3DApplicationSource::CreateView()
{
    return ref new App();
}

void triggerEvent(Windows::UI::Core::PointerEventArgs ^args, ClickEvent type, 
    std::vector<uint>& downClickIds, std::map<uint, Peach3D::Vector2>& downClickPoss)
{
    // fixed dpi
    static float dpi = DisplayInformation::GetForCurrentView()->LogicalDpi;
    const float screenHeight = (float)IPlatform::getSingleton().getCreationParams().height;
    // save clickId list
    uint clickId = args->CurrentPoint->PointerId;
    auto clickIter = std::find(downClickIds.begin(), downClickIds.end(), clickId);
    if (type == ClickEvent::eDown) {
        downClickIds.push_back(clickId);
    }
    else if (type == ClickEvent::eUp && clickIter != downClickIds.end()) {
        downClickIds.erase(clickIter);
    }

    std::vector<uint> clickIds;
    std::vector<Vector2> poss;
    if (type == ClickEvent::eScrollWheel) {
        float scrollY = args->CurrentPoint->Properties->MouseWheelDelta / 120.0f;
        downClickPoss[clickId] = Vector2(0.0f, scrollY);
    }
    else {
        float curPosX = floorf(args->CurrentPoint->Position.X * dpi / 96.0f + 0.5f);
        float curPosY = floorf(args->CurrentPoint->Position.Y * dpi / 96.0f + 0.5f);
        downClickPoss[clickId] = Vector2(curPosX, screenHeight - curPosY);
    }
    // deal multi-touch
    if (type == ClickEvent::eDrag)
    {
        for (size_t i = 0; i < downClickIds.size(); i++)
        {
            uint indexClickId = downClickIds[i];
            poss.push_back(downClickPoss[indexClickId]);
            clickIds.push_back(indexClickId);
        }
    }
    else {
        poss.push_back(downClickPoss[clickId]);
        clickIds.push_back(clickId);
    }
    EventDispatcher::getSingletonPtr()->triggerClickEvent(type, clickIds, poss);
}

App::App() :
m_windowVisible(true)
{
}

// 创建 IFrameworkView 时将调用的第一个方法。
void App::Initialize(CoreApplicationView^ applicationView)
{
    // 注册应用程序生命周期的事件处理程序。此示例包括 Activated，因此我们
    // 可激活 CoreWindow 并开始在窗口上渲染。
    applicationView->Activated +=
        ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &App::OnActivated);

    // back menu event
#if (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
    HardwareButtons::BackPressed += 
        ref new EventHandler<BackPressedEventArgs^>(this, &App::OnBackButtonPressed);
#endif

    CoreApplication::Suspending +=
        ref new EventHandler<SuspendingEventArgs^>(this, &App::OnSuspending);
    CoreApplication::Resuming +=
        ref new EventHandler<Platform::Object^>(this, &App::OnResuming);

    // 初始化平台
    PlatformCreationParams   params;
    params.delegate = &gAppDelegate;
    params.MSAA = 0;
    //params.maxFramsePerSecond = 30.0f;
    mPlatform.initWithParams(params);
}

void App::OnPointerPressed(CoreWindow ^sender, PointerEventArgs ^args)
{
    triggerEvent(args, ClickEvent::eDown, m_downClickIds, m_downClickPoss);
}

void App::OnPointerMoved(CoreWindow ^sender, PointerEventArgs ^args)
{
    if (m_downClickPoss.size() > 0) {
        triggerEvent(args, ClickEvent::eDrag, m_downClickIds, m_downClickPoss);
    }
    else {
        triggerEvent(args, ClickEvent::eMoved, m_downClickIds, m_downClickPoss);
    }
}

void App::OnPointerReleased(CoreWindow ^sender, PointerEventArgs ^args)
{
    triggerEvent(args, ClickEvent::eUp, m_downClickIds, m_downClickPoss);
}

void App::OnPointerWheelChanged(CoreWindow ^sender, PointerEventArgs ^args)
{
    triggerEvent(args, ClickEvent::eScrollWheel, m_downClickIds, m_downClickPoss);
}

void App::OnKeyDown(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::KeyEventArgs ^args)
{
    //args->VirtualKey
    EventDispatcher::getSingletonPtr()->triggerKeyboardEvent(KeyboardEvent::eKeyDown, (Peach3D::KeyCode)args->VirtualKey);
}

void App::OnKeyUp(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::KeyEventArgs ^args)
{
    EventDispatcher::getSingletonPtr()->triggerKeyboardEvent(KeyboardEvent::eKeyUp, (Peach3D::KeyCode)args->VirtualKey);
}

#if (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
void App::OnBackButtonPressed(Platform::Object^ sender, Windows::Phone::UI::Input::BackPressedEventArgs^ args)
{
    args->Handled = true;
    EventDispatcher::getSingletonPtr()->triggerKeyboardEvent(KeyboardEvent::eKeyDown, KeyCode::eBack);
}
#endif

// 创建(或重新创建) CoreWindow 对象时调用。
void App::SetWindow(CoreWindow^ window)
{
    window->VisibilityChanged +=
        ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &App::OnVisibilityChanged);
    window->Closed +=
        ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &App::OnWindowClosed);
    // add touch event
    window->PointerPressed += 
        ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &App::OnPointerPressed);
    window->PointerMoved +=
        ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &App::OnPointerMoved);
    window->PointerReleased +=
        ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &App::OnPointerReleased);
    window->PointerWheelChanged +=
        ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &App::OnPointerWheelChanged);
    // add keyboard event
    window->KeyDown += 
        ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &App::OnKeyDown);
    window->KeyUp +=
        ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &App::OnKeyUp);

    DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();
    DisplayInformation::DisplayContentsInvalidated +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnDisplayContentsInvalidated);

#if !(WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
    window->SizeChanged +=
        ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &App::OnWindowSizeChanged);

    currentDisplayInformation->DpiChanged +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnDpiChanged);

    currentDisplayInformation->OrientationChanged +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnOrientationChanged);

    // 当进行触控时禁用所有指针可视反馈以便获得更好的性能。
    // 这在 Windows Phone 应用程序中不受支持。
    auto pointerVisualizationSettings = PointerVisualizationSettings::GetForCurrentView();
    pointerVisualizationSettings->IsContactFeedbackEnabled = false;
    pointerVisualizationSettings->IsBarrelButtonFeedbackEnabled = false;
#endif

    int width, height, displayRotation;
    getWindowSizeAndRotation(window, &width, &height, &displayRotation);
    // set window and init dx
    mPlatform.setStoreWindow(reinterpret_cast<void*>(window), width, height, (DXGI_MODE_ROTATION)displayRotation);
}

// 初始化场景资源或加载之前保存的应用程序状态。
void App::Load(Platform::String^ entryPoint)
{
}

// 将在窗口处于活动状态后调用此方法。
void App::Run()
{
    while (mPlatform.isRunning())
    {
        if (m_windowVisible)
        {
            CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

            if (mPlatform.isRenderWindowValid())
            {
                // platform will auto decide if need render frame, this param will be discard
                mPlatform.renderOneFrame(0.0f);
            }
        }
        else
        {
            CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
        }
    }
}

// IFrameworkView 所必需的。
// 终止事件不会导致调用 Uninitialize。如果在应用程序在前台运行时销毁 IFrameworkView
// 类，则将调用该方法。
void App::Uninitialize()
{
}

// 应用程序生命周期事件处理程序。

void App::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
    // Run() 在 CoreWindow 激活前将不会开始。
    CoreWindow::GetForCurrentThread()->Activate();
}

void App::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
{
    SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();

    create_task([this, deferral]()
    {
        /** Device will not trigger event when debuging
         * See https://msdn.microsoft.com/en-us/library/windows/apps/hh974425.aspx
         */
        gAppDelegate.appDidEnterBackground();
        deferral->Complete();
    });
}

void App::OnResuming(Platform::Object^ sender, Platform::Object^ args)
{
    /** Device will not trigger event when debuging
     * See https://msdn.microsoft.com/en-us/library/windows/apps/hh974425.aspx
     */
    gAppDelegate.appWillEnterForeground();
}

// 窗口事件处理程序。

void App::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
    m_windowVisible = args->Visible;
}

void App::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
    // exit app
    mPlatform.terminate();
}

#if !(WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
void App::OnWindowSizeChanged(CoreWindow^ sender, WindowSizeChangedEventArgs^ args)
{
    //m_deviceResources->SetLogicalSize(Size(sender->Bounds.Width, sender->Bounds.Height));
    //m_main->CreateWindowSizeDependentResources();
}
#endif

// DisplayInformation 事件处理程序。

void App::OnDisplayContentsInvalidated(DisplayInformation^ sender, Object^ args)
{
    //m_deviceResources->ValidateDevice();
}


#if !(WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
void App::OnDpiChanged(DisplayInformation^ sender, Object^ args)
{
    //m_deviceResources->SetDpi(sender->LogicalDpi);
    //m_main->CreateWindowSizeDependentResources();
}

void App::OnOrientationChanged(DisplayInformation^ sender, Object^ args)
{
    //m_deviceResources->SetCurrentOrientation(sender->CurrentOrientation);
    //m_main->CreateWindowSizeDependentResources();
}
#endif

void App::getWindowSizeAndRotation(CoreWindow^ window, int* width, int* height, int* displayRotation)
{
    // get native window logic size
    Windows::Foundation::Size logicSize = Windows::Foundation::Size(window->Bounds.Width, window->Bounds.Height);

    // get native orientation and current orientation
    DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();
    DisplayOrientations nativeOrientation = currentDisplayInformation->NativeOrientation;
    DisplayOrientations currentOrientation = currentDisplayInformation->CurrentOrientation;
    float dpi = currentDisplayInformation->LogicalDpi;

    // calc pixel window size
    Windows::Foundation::Size pixelSize;
    pixelSize.Width = floorf(logicSize.Width * dpi / 96.0f + 0.5f);
    pixelSize.Height = floorf(logicSize.Height * dpi / 96.0f + 0.5f);
    pixelSize.Width = (pixelSize.Width < 1) ? 1 : pixelSize.Width;
    pixelSize.Height = (pixelSize.Height < 1) ? 1 : pixelSize.Height;

    // set final width and height, decided by rotation
    *displayRotation = computeDisplayRotation(nativeOrientation, currentOrientation);
    bool swapDimensions = (*displayRotation) == DXGI_MODE_ROTATION_ROTATE90 || (*displayRotation) == DXGI_MODE_ROTATION_ROTATE270;
    *width = lround(swapDimensions ? pixelSize.Height : pixelSize.Width);
    *height = lround(swapDimensions ? pixelSize.Width : pixelSize.Height);
}

int App::computeDisplayRotation(DisplayOrientations native, DisplayOrientations current)
{
    DXGI_MODE_ROTATION rotation = DXGI_MODE_ROTATION_UNSPECIFIED;

    // native only can be Landscape or Portrait.
    switch (native)
    {
    case DisplayOrientations::Landscape:
        switch (current)
        {
        case DisplayOrientations::Landscape:
            rotation = DXGI_MODE_ROTATION_IDENTITY;
            break;

        case DisplayOrientations::Portrait:
            rotation = DXGI_MODE_ROTATION_ROTATE270;
            break;

        case DisplayOrientations::LandscapeFlipped:
            rotation = DXGI_MODE_ROTATION_ROTATE180;
            break;

        case DisplayOrientations::PortraitFlipped:
            rotation = DXGI_MODE_ROTATION_ROTATE90;
            break;
        }
        break;

    case DisplayOrientations::Portrait:
        switch (current)
        {
        case DisplayOrientations::Landscape:
            rotation = DXGI_MODE_ROTATION_ROTATE90;
            break;

        case DisplayOrientations::Portrait:
            rotation = DXGI_MODE_ROTATION_IDENTITY;
            break;

        case DisplayOrientations::LandscapeFlipped:
            rotation = DXGI_MODE_ROTATION_ROTATE270;
            break;

        case DisplayOrientations::PortraitFlipped:
            rotation = DXGI_MODE_ROTATION_ROTATE180;
            break;
        }
        break;
    }
    return rotation;
}