//
//  Peach3DPlatformWinUwp.cpp
//  Peach3DLib
//
//  Created by singoon he on 14-9-1.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#include "Peach3DPlatformWinUwp.h"
#include "Peach3DUtils.h"
#include "Peach3DRenderDX.h"
#include "Peach3DTextureDX.h"
#include "Peach3DResourceManager.h"

using namespace Windows::System::Profile;
using namespace Windows::System::UserProfile;
using namespace Windows::Security::ExchangeActiveSyncProvisioning;
namespace Peach3D
{
    PlatformWinUwp::PlatformWinUwp() : mNativeOrientation(DisplayOrientations::None), mCurrentOrientation(DisplayOrientations::None), mDpi(-1.0f)
    {
        mLastRecordTime.QuadPart = 0;

        // get local language
        auto topUserLan = GlobalizationPreferences::Languages->GetAt(0);
        auto displayNameData = topUserLan->Data();
        if (wcsncmp(displayNameData, L"zh", 2) == 0) {
            if (wcsncmp(displayNameData, L"zh-Hant", 7) == 0) {
                mLocalLanguage = LanguageType::eChineseHant;
            }
            else {
                mLocalLanguage = LanguageType::eChineseHans;
            }
        }
        else if (wcsncmp(displayNameData, L"en", 2) == 0) {
            mLocalLanguage = LanguageType::eEnglish;
        }
        else if (wcsncmp(displayNameData, L"fr", 2) == 0) {
            mLocalLanguage = LanguageType::eFrench;
        }
        else if (wcsncmp(displayNameData, L"rn", 2) == 0) {
            mLocalLanguage = LanguageType::eRussian;
        }

        // get OS version string
        auto family = AnalyticsInfo::VersionInfo->DeviceFamily;
        char* utf8Family = convertUnicodeToUTF8((wchar_t*)family->Data());
        auto ai = AnalyticsInfo::VersionInfo->DeviceFamilyVersion;
        long long numVer = _wtoll(ai->Data());
        ulong v1 = (numVer & 0xFFFF000000000000L) >> 48;
        ulong v2 = (numVer & 0x0000FFFF00000000L) >> 32;
        ulong v3 = (numVer & 0x00000000FFFF0000L) >> 16;
        ulong v4 = (numVer & 0x000000000000FFFFL);
        mOSVerStr = Utils::formatString("%s %d.%d.%d.%d", utf8Family, v1, v2, v3, v4);
        free(utf8Family);
        // get device model string
        auto deviceInfo = ref new EasClientDeviceInformation();
        auto modelStr = deviceInfo->SystemManufacturer + L" " + deviceInfo->SystemProductName;
        char* utf8Model = convertUnicodeToUTF8((wchar_t*)modelStr->Data());
        mDeviceModel = utf8Model;
        free(utf8Model);

        // get writeable path
		Platform::String^ writePath = Windows::Storage::ApplicationData::Current->LocalFolder->Path;
        char* utf8Dir = convertUnicodeToUTF8((wchar_t*)writePath->Data());
        mWriteablePath = utf8Dir;
        if (mWriteablePath[mWriteablePath.size() - 1] != '\\') {
            mWriteablePath = mWriteablePath + "\\";
        }
        free(utf8Dir);
    }

    PlatformWinUwp::~PlatformWinUwp()
    {
        // render will auto delete in IPlatform, so do nothing here
    }

    bool PlatformWinUwp::initWithParams(const PlatformCreationParams &params)
    {
        // setup time record
        QueryPerformanceFrequency(&mFrequency);
        // init base Platform
        IPlatform::initWithParams(params);

        // create Render and init global handlers
        mRender = new RenderDX();
        RenderDX* nativeRender = static_cast<RenderDX*>(mRender);
        bool globalSuccess = nativeRender->createRender();
        // save render feature level
        mFeatureLevel = RenderFeatureLevel::eDX;

        // get default resource directory
        Platform::String^ accessPath = Windows::ApplicationModel::Package::Current->InstalledLocation->Path;
        char* utf8String = convertUnicodeToUTF8((wchar_t*)accessPath->Data());
        ResourceManager::getSingleton().addSearchDirectory(utf8String);
        free(utf8String);

        return globalSuccess;
    }

    void PlatformWinUwp::setStoreWindow(void* window)
    {
        IPlatform::setWindow(window);

        DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();
        auto winHandler = reinterpret_cast<Windows::UI::Core::CoreWindow^>(mCreationParams.window);
        mLogicalSize = Windows::Foundation::Size(winHandler->Bounds.Width, winHandler->Bounds.Height);
        mNativeOrientation = currentDisplayInformation->NativeOrientation;
        mCurrentOrientation = currentDisplayInformation->CurrentOrientation;
        mDpi = currentDisplayInformation->LogicalDpi;

        // update render device and sreen size.
        bool userSuccess = initRenderDX();
        if (userSuccess) {
            // notify IAppDelegate launch finished
            userSuccess = mCreationParams.delegate->appDidFinishLaunching();

            if (userSuccess) {
                // start animating
                mAnimating = true;
            }
        }
    }

    void PlatformWinUwp::setLogicalSize(Windows::Foundation::Size logicalSize)
    {
        if (mLogicalSize != logicalSize) {
            mLogicalSize = logicalSize;

            // update render device and sreen size.
            initRenderDX();
        }
    }

    void PlatformWinUwp::setCurrentOrientation(DisplayOrientations currentOrientation)
    {
        if (mCurrentOrientation != currentOrientation) {
            mCurrentOrientation = currentOrientation;

            // update render device and sreen size.
            // notify app delegate window size changed
            if (this->initRenderDX()) {
                mCreationParams.delegate->appWindowSizeChanged();
            }
        }
    }

    void PlatformWinUwp::setDpi(float dpi)
    {
        if (dpi != mDpi) {
            mDpi = dpi;
            // When the display DPI changes, the logical size of the window (measured in Dips) also changes and needs to be updated.
            auto winHandler = reinterpret_cast<Windows::UI::Core::CoreWindow^>(mCreationParams.window);
            mLogicalSize = Windows::Foundation::Size(winHandler->Bounds.Width, winHandler->Bounds.Height);

            // update render device and sreen size.
            // notify app delegate window size changed
            if (this->initRenderDX()) {
                mCreationParams.delegate->appWindowSizeChanged();
            }
        }
    }

    void PlatformWinUwp::renderOneFrame(float lastFrameTime)
    {
        static double perFrameTime = 1 / mCreationParams.maxFPS;
        static double curDeltaTime = 0.0;
        // get current time and delta
        LARGE_INTEGER currentTime;
        QueryPerformanceCounter(&currentTime);

        if (mLastRecordTime.QuadPart > 0) {
            double deltaTime = double(currentTime.QuadPart - mLastRecordTime.QuadPart) / (mFrequency.QuadPart);
            curDeltaTime += deltaTime;
            // if delta time bigger than 1/FPS, render frame.
            if (curDeltaTime >= perFrameTime) {
                IPlatform::renderOneFrame(float(curDeltaTime));
                curDeltaTime = 0;
            }
        }
        mLastRecordTime = currentTime;
    }

    void PlatformWinUwp::resumeAnimating()
    {
        if (!mAnimating) {
            IPlatform::resumeAnimating();
            // reset last time
            mLastRecordTime.QuadPart = 0;
        }
    }

    void PlatformWinUwp::displayInvalidExit()
    {
        // device last, auto exit
        auto exitDia = ref new Windows::UI::Popups::MessageDialog(L"Device last, auto exit!");
        exitDia->ShowAsync();
        terminate();
    }

    ITexture* PlatformWinUwp::getTextTexture(const std::vector<LabelStageTextInfo>& textList, const LabelTextDefined& defined,
        std::map<std::string, std::vector<Rect>>& clicksRect)
    {
        char pName[100] = { 0 };
        static uint texAutoCount = 0;
        sprintf(pName, "pd_DXTexture%d", texAutoCount++);
        // create texture with inner name
        TextureDX *texture = static_cast<TextureDX*>(IRender::getSingleton().createTexture(pName));
        if (!texture->createTextTexture(textList, defined, clicksRect)) {
            ResourceManager::getSingleton().deleteTexture(texture);
            texture = nullptr;
        }
        return texture;
    }

    void PlatformWinUwp::openUrl(const std::string& url)
    {
        if (url.size()==0) return;
        
        // auto add "http://" header
        std::string finalUrl = url;
        if (finalUrl.find("http://")==std::string::npos) {
            finalUrl = "http://" + finalUrl;
        }
        wchar_t* wUrl = convertUTF8ToUnicode(finalUrl);
        Platform::String^ ns = ref new Platform::String(wUrl);
        free(wUrl);
        Windows::Foundation::Uri^ uri = ref new Windows::Foundation::Uri(ns);
        Windows::System::Launcher::LaunchUriAsync(uri);
    }

    bool PlatformWinUwp::initRenderDX()
    {
        // The width and height of the swap chain must be based on the window's
        // natively-oriented width and height. If the window is not in the native
        // orientation, the dimensions must be reversed.
        DXGI_MODE_ROTATION displayRotation = computeDisplayRotation();
        // update game size and rotation, only support rotation 0 and 180
        if (displayRotation != DXGI_MODE_ROTATION_UNSPECIFIED) {
            static const float dipsPerInch = 96.0f;
            // Calculate the necessary render target size in pixels.
            float tWidth = floorf(mLogicalSize.Width * mDpi / dipsPerInch + 0.5f);
            float tHeight = floorf(mLogicalSize.Height * mDpi / dipsPerInch + 0.5f);

            // Prevent zero size DirectX content from being created.
            tWidth = max(tWidth, 1);
            tHeight = max(tHeight, 1);
            mCreationParams.winSize = Vector2(tWidth, tHeight);

            return static_cast<RenderDX*>(mRender)->initRender(mCreationParams.winSize, displayRotation);
        }
        return false;
    }

    DXGI_MODE_ROTATION PlatformWinUwp::computeDisplayRotation()
    {
        DXGI_MODE_ROTATION rotation = DXGI_MODE_ROTATION_UNSPECIFIED;

        // Note: NativeOrientation can only be Landscape or Portrait even though
        // orientation only support 0 and 180.
        switch (mNativeOrientation) {
        case DisplayOrientations::Landscape:
            switch (mCurrentOrientation) {
            case DisplayOrientations::Landscape:
                rotation = DXGI_MODE_ROTATION_IDENTITY;
                break;
            case DisplayOrientations::LandscapeFlipped:
                rotation = DXGI_MODE_ROTATION_ROTATE180;
                break;
            }
            break;
        case DisplayOrientations::Portrait:
            switch (mCurrentOrientation) {
            case DisplayOrientations::Portrait:
                rotation = DXGI_MODE_ROTATION_IDENTITY;
                break;
            case DisplayOrientations::PortraitFlipped:
                rotation = DXGI_MODE_ROTATION_ROTATE180;
                break;
            }
            break;
        }
        return rotation;
    }
}