#include "App.h"
#include "PlatformDelegate.h"

#include <ppltasks.h>

using namespace test;
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

// The DirectX 12 Application template is documented at http://go.microsoft.com/fwlink/?LinkID=613670&clcid=0x409

// Peach3D delegate class
PlatformDelegate gAppDelegate;
// The main function is only used to initialize our IFrameworkView class.
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

App::App() :
	m_windowVisible(true)
{
}

// The first method called when the IFrameworkView is being created.
void App::Initialize(CoreApplicationView^ applicationView)
{
	// Register event handlers for app lifecycle. This example includes Activated, so that we
	// can make the CoreWindow active and start rendering on the window.
	applicationView->Activated +=
		ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &App::OnActivated);

	CoreApplication::Suspending +=
		ref new EventHandler<SuspendingEventArgs^>(this, &App::OnSuspending);

	CoreApplication::Resuming +=
		ref new EventHandler<Platform::Object^>(this, &App::OnResuming);

	// initialize platform once
	PlatformCreationParams params;
	params.delegate = &gAppDelegate;
	params.MSAA = 0;
	//params.maxFramsePerSecond = 30.0f;
	mPlatform.initWithParams(params);
}

// Called when the CoreWindow object is created (or re-created).
void App::SetWindow(CoreWindow^ window)
{
	window->SizeChanged += 
		ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &App::OnWindowSizeChanged);

	window->VisibilityChanged +=
		ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &App::OnVisibilityChanged);

	window->Closed += 
		ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &App::OnWindowClosed);

	DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

	currentDisplayInformation->DpiChanged +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnDpiChanged);

	currentDisplayInformation->OrientationChanged +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnOrientationChanged);

	DisplayInformation::DisplayContentsInvalidated +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnDisplayContentsInvalidated);

	// set window and init dx
	mPlatform.setStoreWindow(reinterpret_cast<void*>(window));
}

// Initializes scene resources, or loads a previously saved app state.
void App::Load(Platform::String^ entryPoint)
{
}

// This method is called after the window becomes active.
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

// Required for IFrameworkView.
// Terminate events do not cause Uninitialize to be called. It will be called if your IFrameworkView
// class is torn down while the app is in the foreground.
void App::Uninitialize()
{
}

// Application lifecycle event handlers.

void App::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
	// Run() won't start until the CoreWindow is activated.
	CoreWindow::GetForCurrentThread()->Activate();
}

void App::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
{
	// Save app state asynchronously after requesting a deferral. Holding a deferral
	// indicates that the application is busy performing suspending operations. Be
	// aware that a deferral may not be held indefinitely. After about five seconds,
	// the app will be forced to exit.
	SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();

	create_task([this, deferral]()
	{
        // call delegate callback function
        gAppDelegate.appDidEnterBackground();

		deferral->Complete();
	});
}

void App::OnResuming(Platform::Object^ sender, Platform::Object^ args)
{
	// Restore any data or state that was unloaded on suspend. By default, data
	// and state are persisted when resuming from suspend. Note that this event
	// does not occur if the app was previously terminated.

	// call delegate callback functiond();
    gAppDelegate.appWillEnterForeground();
}

// Window event handlers.

void App::OnWindowSizeChanged(CoreWindow^ sender, WindowSizeChangedEventArgs^ args)
{
    mPlatform.setLogicalSize(Size(sender->Bounds.Width, sender->Bounds.Height));
}

void App::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
	m_windowVisible = args->Visible;
}

void App::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
	// exit app
    mPlatform.terminate();
}

// DisplayInformation event handlers.

void App::OnDpiChanged(DisplayInformation^ sender, Object^ args)
{
    mPlatform.setDpi(sender->LogicalDpi);
}

void App::OnOrientationChanged(DisplayInformation^ sender, Object^ args)
{
    mPlatform.setCurrentOrientation(sender->CurrentOrientation);
}

void App::OnDisplayContentsInvalidated(DisplayInformation^ sender, Object^ args)
{
    // device last, auto exit
    mPlatform.displayInvalidExit();
}