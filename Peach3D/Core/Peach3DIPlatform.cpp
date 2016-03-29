//
//  Peach3DPlatform.cpp
//  TestPeach3D
//
//  Created by singoon he on 12-4-15.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#include "Peach3DIPlatform.h"
#include "Peach3DCompile.h"
#include "Peach3DResourceManager.h"
#include "Peach3DSceneManager.h"
#include "Peach3DIScene.h"
#include "Peach3DEventDispatcher.h"
#include "Peach3DLayoutManager.h"
#include "Peach3DCursorManager.h"

namespace Peach3D
{
    IMPLEMENT_SINGLETON_STATIC(IPlatform);
    
    IPlatform::IPlatform() :mRender(nullptr), mCurrentFPS(0), mDrawCount(0), mTriangleCount(0), mAnimating(false), mTerminating(false),
    mResourceMgr(nullptr), mFeatureLevel(RenderFeatureLevel::eUnkonw), mSceneManager(nullptr), mEventDispatcher(nullptr),
    mActiveScene(nullptr), mIsDrawStats(false), mLocalLanguage(LanguageType::eUnknow), mLayoutManager(nullptr), mCursorManager(nullptr)
    {
    }

    IPlatform::~IPlatform()
    {
        // delete layout manager
        delete mLayoutManager;
        // deelte event dispatcher
        delete mEventDispatcher;
        mEventDispatcher = nullptr;
        // delete all scheduler
        for (Scheduler* item : mSchedulerList) {
            delete item;
        }
        mSchedulerList.clear();
        for (Scheduler* item : mAddSchedulerList) {
            delete item;
        }
        mAddSchedulerList.clear();
        mLogPrinter->print(LogLevel::eInfo, "Peach3D exit");
        // delete log manager
        delete mLogPrinter;
        mLogPrinter = nullptr;
    }
    
    void IPlatform::terminate()
    {
        mTerminating = true;
        mAnimating = false;
        
        /** Delete render resource, ~IPlatform may EGL invalid. */
        // Delete cursor manager before scene manager, it handle sprite.
        if (mCursorManager) {
            delete mCursorManager;
            mCursorManager = nullptr;
        }
        // delete scene manager
        if (mSceneManager) {
            delete mSceneManager;
            mSceneManager = nullptr;
            mLogPrinter->print(LogLevel::eInfo, "Scene manager exit");
        }
        // delete resource manager
        if (mResourceMgr) {
            delete mResourceMgr;
            mResourceMgr = nullptr;
            mLogPrinter->print(LogLevel::eInfo, "Resource manager exit");
        }
        // delete render
        if (mRender) {
            delete mRender;
            mRender = nullptr;
            mLogPrinter->print(LogLevel::eInfo, "IRender exit");
        }
    }

    bool IPlatform::initWithParams(const PlatformCreationParams &params)
    {
        mCreationParams = params;
        // check params
        Peach3DAssert(params.delegate!=nullptr, "IAppDelegate must be set!");
        
        // create log manager
        mLogPrinter = new LogPrinter(mCreationParams.logPath);
        // print Peach3D version
        mLogPrinter->print(LogLevel::eInfo, "Peach3D version: %s", PEACH3D_SDK_VERSION);
        // print OS version
        mLogPrinter->print(LogLevel::eInfo, "OS version: %s", mOSVerStr.c_str());
        // print local language
        mLogPrinter->print(LogLevel::eInfo, "Local language: %d", mLocalLanguage);
        // print device model
        mLogPrinter->print(LogLevel::eInfo, "Device model: %s", mDeviceModel.c_str());
        // create resource manager
        mResourceMgr = new ResourceManager();
        // create scene manager and init
        mSceneManager = new SceneManager();
        // create event dispatcher
        mEventDispatcher = new EventDispatcher();
        // create layout manager
        mLayoutManager = new LayoutManager();
        // create cursor manager
        mCursorManager = new CursorManager();

        return true;
    }
    
    void IPlatform::replaceWithNewScene(IScene* newScene)
    {
        // init or reset scene manager
        if (!mActiveScene) {
            mSceneManager->init();
        }
        else {
            mSceneManager->reset();
        }
        // exit old scene
        if (mActiveScene) {
            mActiveScene->exit();
            delete mActiveScene;
            mActiveScene = nullptr;
        }
        // set new scene active
        if (newScene->init()) {
            mActiveScene = newScene;
        }
    }

    void IPlatform::renderOneFrame(float lastFrameTime)
    {
        if (!mAnimating) {
            return;
        }
        // update the FPS count, it will clear when return FPS.
        mCurrentFPS++;
        
        // add new scheduler
        if (mAddSchedulerList.size() > 0) {
            for (Scheduler* item : mAddSchedulerList) {
                mSchedulerList.push_back(item);
            }
            mAddSchedulerList.clear();
        }
        // call all scheduler
        for (auto iter=mSchedulerList.begin(); iter!=mSchedulerList.end();) {
            if (!(*iter)->isPaused()) {
                (*iter)->trigger(lastFrameTime);
                
                // auto delete invalid scheduler
                if ((*iter)->getRepeatCount() == 0) {
                    delete (*iter);
                    iter = mSchedulerList.erase(iter);
                    continue;
                }
            }
            ++iter;
        }
        // update current active scene and render
        if (mActiveScene) {
            mActiveScene->update(lastFrameTime);
            
            // clean draw count and triangle count
            mDrawCount = 0;
            mTriangleCount = 0;
            mSceneManager->render(lastFrameTime);
        }
    }
    
    Scheduler* IPlatform::addScheduler(std::function<void(float)> callback, float interval, int repeatCount, bool autoStart)
    {
        Scheduler* newScheduler = new Scheduler(callback, interval, repeatCount);
        if (autoStart) {
            newScheduler->start();
        }
        // add to temp list, it will add to mSchdulerList in next loop.
        mAddSchedulerList.push_back(newScheduler);
        return newScheduler;
    }
    
    void IPlatform::deleteScheduler(Scheduler* scheduler)
    {
        // scheduler will auto deleted in loop
        if (scheduler) {
            scheduler->setRepeatCount(0);
        }
    }
}
