#pragma once

#include "WinStore\Peach3DPlatformWinStore.h"

namespace Test
{
    // 我们的应用程序的主入口点。使用 Windows shell 连接应用程序并处理应用程序生命周期事件。
    ref class App sealed : public Windows::ApplicationModel::Core::IFrameworkView
    {
    public:
        App();

        // IFrameworkView 方法。
        virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView);
        virtual void SetWindow(Windows::UI::Core::CoreWindow^ window);
        virtual void Load(Platform::String^ entryPoint);
        virtual void Run();
        virtual void Uninitialize();

    protected:
        // 应用程序生命周期事件处理程序。
        void OnActivated(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView, Windows::ApplicationModel::Activation::IActivatedEventArgs^ args);
        void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ args);
        void OnResuming(Platform::Object^ sender, Platform::Object^ args);
#if (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
        void OnBackButtonPressed(Platform::Object^ sender, Windows::Phone::UI::Input::BackPressedEventArgs^ args);
#endif
        // touch event
        void OnPointerPressed(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::PointerEventArgs ^args);
        void OnPointerMoved(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::PointerEventArgs ^args);
        void OnPointerReleased(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::PointerEventArgs ^args);
        void OnPointerWheelChanged(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::PointerEventArgs ^args);
        void OnKeyDown(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::KeyEventArgs ^args);
        void OnKeyUp(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::KeyEventArgs ^args);

        // 窗口事件处理程序。
#if !(WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
        void OnWindowSizeChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ args);
#endif
        void OnVisibilityChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args);
        void OnWindowClosed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::CoreWindowEventArgs^ args);

        // DisplayInformation 事件处理程序。
#if !(WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
        void OnDpiChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
        void OnOrientationChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
#endif
        void OnDisplayContentsInvalidated(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);

        // get window size and rotation degree
        void getWindowSizeAndRotation(Windows::UI::Core::CoreWindow^ window, int* width, int* height, int* displayRotation);
        int computeDisplayRotation(Windows::Graphics::Display::DisplayOrientations native, Windows::Graphics::Display::DisplayOrientations current);

    private:
        Peach3D::PlatformWinStore mPlatform;

        bool    m_windowVisible;

        std::vector<uint>                m_downClickIds;     // clickId list
        std::map<uint, Peach3D::Vector2> m_downClickPoss;    // click pos list
    };
}

ref class Direct3DApplicationSource sealed : Windows::ApplicationModel::Core::IFrameworkViewSource
{
public:
    virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView();
};
