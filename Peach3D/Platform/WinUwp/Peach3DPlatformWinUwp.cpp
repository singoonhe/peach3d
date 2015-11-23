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
    PlatformWinUwp::PlatformWinUwp()
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

    void PlatformWinUwp::setStoreWindow(void* window, int width, int height, DXGI_MODE_ROTATION rotation)
    {
        IPlatform::setWindow(window);
        // save created window size
        mCreationParams.width = width;
        mCreationParams.height = height;

        // at last, init render after get final window size
        bool success = mRender->initRender(mCreationParams.width, mCreationParams.height);
        bool userSuccess = true;
        if (success) {
            // set swap chain rotation and projective rotation
            RenderDX* nativeRender = static_cast<RenderDX*>(mRender);
            nativeRender->setContentOritation(rotation);

            // notify IAppDelegate launch finished
            userSuccess = mCreationParams.delegate->appDidFinishLaunching();

            if (userSuccess) {
                // start animating
                mAnimating = true;
            }
        }
    }

    void PlatformWinUwp::renderOneFrame(float lastFrameTime)
    {
        static double perFrameTime = 1 / mCreationParams.maxFramsePerSecond;
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
}