//
//  Peach3DPlatformWinUwp.cpp
//  Peach3DLib
//
//  Created by singoon he on 14-9-1.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#include "Peach3DPlatformWinUwp.h"
#include "Peach3DRenderDX.h"
#include "Peach3DTextureDX.h"
#include "Peach3DResourceManager.h"

namespace Peach3D
{
    PlatformWinUwp::PlatformWinUwp()
    {
        mLastRecordTime.QuadPart = 0;

        // get local language
#if WINAPI_FAMILY==WINAPI_FAMILY_PHONE_APP
        ULONG numLanguages = 0;
        DWORD cchLanguagesBuffer = 0;
        BOOL hr = GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &numLanguages, NULL, &cchLanguagesBuffer);
        if (hr) {
            WCHAR* pwszLanguagesBuffer = new WCHAR[cchLanguagesBuffer];
            hr = GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &numLanguages, pwszLanguagesBuffer, &cchLanguagesBuffer);
            if (hr) {
                if (wcsncmp(pwszLanguagesBuffer, L"zh", 2) == 0) {
                    if (wcsncmp(pwszLanguagesBuffer, L"zh-CN", 5) == 0) {
                        mLocalLanguage = LanguageType::eChineseHans;
                    }
                    else {
                        mLocalLanguage = LanguageType::eChineseHant;
                    }
                }
                else if (wcsncmp(pwszLanguagesBuffer, L"en", 2) == 0) {
                    mLocalLanguage = LanguageType::eEnglish;
                }
                else if (wcsncmp(pwszLanguagesBuffer, L"fr", 2) == 0) {
                    mLocalLanguage = LanguageType::eFrench;
                }
                else if (wcsncmp(pwszLanguagesBuffer, L"rn", 2) == 0) {
                    mLocalLanguage = LanguageType::eRussian;
                }
            }
            delete[] pwszLanguagesBuffer;
        }
        // get OS version string
        mOSVerStr = "Windows Phone 8.1";
#else
        WCHAR languageBuffer[256];
        int ret = GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, 
            LOCALE_SENGLISHLANGUAGENAME,
            languageBuffer,
            256);
        if (ret) {
            if (wcsncmp(languageBuffer, L"Chinese", wcslen(L"Chinese")) == 0) {
                if (wcsncmp(languageBuffer, L"Chinese (Simplified)", wcslen(L"Chinese (Simplified)")) == 0) {
                    mLocalLanguage = LanguageType::eChineseHans;
                }
                else {
                    mLocalLanguage = LanguageType::eChineseHant;
                }
            }
            else if (wcsncmp(languageBuffer, L"English", wcslen(L"English")) == 0) {
                mLocalLanguage = LanguageType::eEnglish;
            }
            else if (wcsncmp(languageBuffer, L"French", wcslen(L"French")) == 0) {
                mLocalLanguage = LanguageType::eFrench;
            }
            else if (wcsncmp(languageBuffer, L"Russian", wcslen(L"Russian")) == 0) {
                mLocalLanguage = LanguageType::eRussian;
            }
        }
        // get OS version string
        mOSVerStr = "Windows 8.1";
#endif

        // get writeable path
        Platform::String^ writePath = Windows::Storage::ApplicationData::Current->LocalFolder->Path;
        char* utf8String = convertUnicodeToUTF8((wchar_t*)writePath->Data());
        mWriteablePath = utf8String;
        if (mWriteablePath[mWriteablePath.size() - 1] != '\\') {
            mWriteablePath = mWriteablePath + "\\";
        }
        free(utf8String);
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
        bool globalSuccess = nativeRender->createGlobelRender();
        // save render feature level
        mFeatureLevel = RenderFeatureLevel::eDX11;

        // get default resource directory
        Platform::String^ accessPath = Package::Current->InstalledLocation->Path;
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
        if (success)
        {
            // set swap chain rotation and projective rotation
            RenderDX* nativeRender = static_cast<RenderDX*>(mRender);
            nativeRender->setContentOritation(rotation);

            // notify IAppDelegate launch finished
            userSuccess = mCreationParams.delegate->appDidFinishLaunching();

            if (userSuccess)
            {
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

        if (mLastRecordTime.QuadPart > 0)
        {
            double deltaTime = double(currentTime.QuadPart - mLastRecordTime.QuadPart) / (mFrequency.QuadPart);
            curDeltaTime += deltaTime;
            // if delta time bigger than 1/FPS, render frame.
            if (curDeltaTime >= perFrameTime)
            {
                IPlatform::renderOneFrame(float(curDeltaTime));
                curDeltaTime = 0;
            }
        }
        mLastRecordTime = currentTime;
    }

    void PlatformWinUwp::resumeAnimating()
    {
        if (!mAnimating)
        {
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
        if (finalUrl.find("http://")==std::string::npos)
        {
            finalUrl = "http://" + finalUrl;
        }
        wchar_t* wUrl = convertUTF8ToUnicode(finalUrl);
        Platform::String^ ns = ref new Platform::String(wUrl);
        free(wUrl);
        Windows::Foundation::Uri^ uri = ref new Windows::Foundation::Uri(ns);
        Windows::System::Launcher::LaunchUriAsync(uri);
    }
}