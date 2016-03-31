//
//  Peach3DPlatformWinDesk.cpp
//  Peach3DLib
//
//  Created by singoon he on 14-9-1.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#include "Peach3DPlatformWinDesk.h"
#include "Peach3DRenderDX.h"
#include "Peach3DTextureDX.h"
#include "Peach3DResourceManager.h"
#include "Peach3DUtils.h"

namespace Peach3D
{
    PlatformWinDesk::PlatformWinDesk()
    {
        mLastRecordTime.QuadPart = 0;

        // get local language
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
        OSVERSIONINFOEX osver;
        osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
        if (GetVersionEx((LPOSVERSIONINFOA)&osver)) {
            mOSVerStr = Utils::formatString("%d.%d Build %d Service Pack %d.%d", osver.dwMajorVersion, osver.dwMinorVersion,
                osver.dwBuildNumber, osver.wServicePackMajor, osver.wServicePackMinor);
        }

        // set runtime sub document dir to write path
        TCHAR Buffer[256];
        GetCurrentDirectory(256, Buffer);
        mWriteablePath = Buffer;
        if (mWriteablePath[mWriteablePath.size() - 1] != '\\') {
            mWriteablePath = mWriteablePath + "\\";
        }
        mWriteablePath = mWriteablePath + "Document\\";
        // create document dir
        CreateDirectory(mWriteablePath.c_str(), NULL);
    }

    PlatformWinDesk::~PlatformWinDesk()
    {
    }

    bool PlatformWinDesk::initWithParams(const PlatformCreationParams &params)
    {
        // setup time record
        QueryPerformanceFrequency(&mFrequency);
        // init base Platform
		IPlatform::initWithParams(params);
		// add default search file dir
		ResourceManager::getSingleton().addSearchDirectory(".\\");

        // create Render and init global handlers
        mRender = new RenderDX();
		do
		{
			RenderDX* nativeRender = static_cast<RenderDX*>(mRender);
			if (!nativeRender->createGlobelRender())
			{
				break;
			}
			// save render feature level
			mFeatureLevel = RenderFeatureLevel::eDX11;

			// at last, init render after get final window size
			if (!mRender->initRender(mCreationParams.width, mCreationParams.height))
			{
				break;
			}
			// notify IAppDelegate launch finished
			if (!mCreationParams.delegate->appDidFinishLaunching())
			{
				break;
			}
			// start animating
			mAnimating = true;
		} while (0);
		return mAnimating;
    }

    void PlatformWinDesk::renderOneFrame(float lastFrameTime)
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

    void PlatformWinDesk::resumeAnimating()
    {
        if (!mAnimating)
        {
            IPlatform::resumeAnimating();
            // reset last time
            mLastRecordTime.QuadPart = 0;
        }
    }

    bool PlatformWinDesk::isFlagKeyDown(KeyCode flagKey)
    {
        // KeyCode is equal to windows key
        int winFlag = (int)flagKey;
        if (winFlag > 0) {
            return GetAsyncKeyState(winFlag)>0;
        }
        return false;
    }

    TexturePtr PlatformWinDesk::getTextTexture(const std::vector<LabelStageTextInfo>& textList, const LabelTextDefined& defined,
        std::map<std::string, std::vector<Rect>>& clicksRect)
    {
        char pName[100] = { 0 };
        static uint texAutoCount = 0;
        sprintf(pName, "pd_DXTexture%d", texAutoCount++);
        // create texture with inner name
        TextureDX *texture = static_cast<TextureDX*>(IRender::getSingletonPtr()->createTexture(pName));
        if (!texture->createTextTexture(textList, defined, clicksRect)) {
            ResourceManager::getSingleton().deleteTexture(texture);
            texture = nullptr;
        }
        return texture;
    }


    void PlatformWinDesk::openUrl(const std::string& url)
    {
        if (url.size()==0) return;

        // auto add "http://" header
        std::string finalUrl = url;
        if (finalUrl.find("http://")==std::string::npos)
        {
            finalUrl = "http://" + finalUrl;
        }
        ShellExecute(NULL, "open", finalUrl.c_str(), NULL, NULL, SW_SHOWNORMAL);
    }
}