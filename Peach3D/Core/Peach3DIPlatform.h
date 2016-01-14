//
//  Peach3DPlatform.h
//  Peach3DLib
//
//  Created by singoon he on 12-4-15.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#ifndef PEACH3D_PLATFORM_H
#define PEACH3D_PLATFORM_H

#include "Peach3DCompile.h"
#include "Peach3DIRender.h"
#include "Peach3DLogPrinter.h"
#include "Peach3DEventDispatcher.h"
#include "Peach3DIAppDelegate.h"
#include "Peach3DRect.h"
#include "Peach3DTypes.h"

namespace Peach3D
{
    ///////////////////////////////////////////////////////
    //////////////////// Creation Params  /////////////////
    ///////////////////////////////////////////////////////
    struct PEACH3D_DLL PlatformCreationParams
    {
        PlatformCreationParams() :
            winSize(800.0f, 600.0f),
            zBits(24),
            sBits(8),
            MSAA(0),
            maxFPS(60.0f),
            window(0),
            logPath(""),
            delegate(nullptr)
        {}

        PlatformCreationParams& operator=(const PlatformCreationParams& other)
        {
            winSize = other.winSize;
            zBits = other.zBits;
            sBits = other.sBits;
            MSAA = other.MSAA;
            maxFPS = other.maxFPS;
            window = other.window;
            logPath = other.logPath;
            delegate = other.delegate;
            return *this;
        }

        //! window size
        Vector2 winSize;
        //! depth buffer bits,16 or 24.
        int zBits;
        //! stencil buffer bits, 0 or 8. zBits must be 0 if zBits set to 16 on Windows.
        int sBits;
        //!　Multi-Sample Anti-Aliasing, ios: 4 or 0. android: 1 or 0.
        int MSAA;
        //! the max frames per second
        float maxFPS;
        //! render window pointer
        void *window;
        //!　the file path of log, auto add writeable dir if relative path
        std::string logPath;
        //! application running delegate
        IAppDelegate* delegate;
    };
    
    /////////////////////language type////////////////////
    enum class PEACH3D_DLL LanguageType
    {
        eUnknow,        // unknow language
        eEnglish,
        eChineseHans,   // Simplified Chinese
        eChineseHant,   // Traditional Chinese
        eFrench,
        eRussian,
    };

    class SceneManager;
    class ResourceManager;
    class LayoutManager;
    class PEACH3D_DLL IPlatform : public Singleton < IPlatform >
    {
    public:
        IPlatform();
        virtual ~IPlatform();

        //! base platform will be initialized with params.
        /** 
         * @params the params that defined above this file.
         * @return the window of the render frame displayed.
         */
        virtual bool initWithParams(const PlatformCreationParams &params);
        //! return the platform initialized params.
        PlatformCreationParams& getCreationParams(){ return mCreationParams; }
        
        //! replace with new scene, this will delete old scene and clear scene manager
        void replaceWithNewScene(IScene* newScene);
        //! get current running scene
        IScene* getRunningScene()const {return mActiveScene;}
        
        //! is flag key down
        virtual bool isFlagKeyDown(KeyCode flagKey) {return false;}
        //! get label text texture
        virtual ITexture* getTextTexture(const std::vector<LabelStageTextInfo>& textList, const LabelTextDefined& defined,
                                         std::map<std::string, std::vector<Rect>>& clicksRect) {return nullptr;}

        //! pause animating
        virtual void pauseAnimating() { mAnimating = false; }
        //! resume animating
        virtual void resumeAnimating() { mAnimating = true; }
        /** Terminate application, it's needed to trigger exit in android and windesk. */
        virtual void terminate() { mTerminating = true; mAnimating = false; }

        //! Get render window is renderable.
        virtual bool isRenderWindowValid() { return mRender->isRenderValid(); }
        //! swap two buffers, calculate FPS.
        /** \return the time of last frame rendered time.*/
        virtual void renderOneFrame(float lastFrameTime);
        
        /**
         * Add a scheduler, user need call deleteScheduler if repeatCount not modified.
         * @params repeatCount Scheduler will auto delete if repeatCount > 0.
         */
        Scheduler* addScheduler(std::function<void(float)> callback, float interval=0.0f, int repeatCount=-1, bool autoStart=true);
        //! delete a scheduler
        void deleteScheduler(Scheduler* scheduler);
        /** Perform function with delay time, callback just work once and auto delete scheduler. */
        void performWithDelay(std::function<void(float)> callback, float delay) { addScheduler(callback, delay, 1); }
        //! set draw stats is need display
        void setIsDrawStats(bool show) {mIsDrawStats = show;}
        //! get draw stats is need display
        bool getIsDrawStats() {return mIsDrawStats;}
        
        //! return the render FPS, this will clear the FPS count.
        int getCurrentFPSAndClear() { int fps = mCurrentFPS; mCurrentFPS = 0; return fps; };
        //! return draw call count per frame
        int getDrawCount(){ return mDrawCount; };
        //! return trangle
        int getTriangleCount(){ return mTriangleCount; };
        //! add draw call count
        void addDrawCallCount(int count) { mDrawCount += count; }
        //! add draw triangle count
        void addDrawTriangleCount(int count) { mTriangleCount += count; }
        
        /**
         * Return render feature level, it must be set in child class.
         * Notice: GL3 include OpenGL 3.x, OpenGL 4.x and OpenGL ES 3.0, GL2 define for OpenGL 2.x and OpenGL ES 2.0.
         */
        RenderFeatureLevel getRenderFeatureLevel() { return mFeatureLevel; }
        /** Return current local language type, it's set in PlatformXXX. */
        LanguageType getLocalLanguage() {return mLocalLanguage;}
        /** Return current OS version string, it's set in PlatformXXX */
        std::string getOSVersionString() {return mOSVerStr;}
        /** Open url, "http://" is not necessary. */
        virtual void openUrl(const std::string& url) = 0;
        /**
         * Return write able path.
         * Windows will set to runtime dir, android set to "sdcard/.com.xxx.xxx" and mac set to "application support/com.xxx.xxx". 
         */
        std::string getWriteablePath() {return mWriteablePath;}
        
    protected:
        // inherited platform save new window
        virtual void setWindow(void* window) { mCreationParams.window = window; }
        /** Delete render dependency, delete EGL or something. */
        virtual void deleteRenderDependency() {}
        
    protected:
        bool                mAnimating;             // application animate
        bool                mTerminating;           // application terminate
        bool                mIsDrawStats;           // is show drawing stats, include FPS/draw count...
        int                 mCurrentFPS;            // current render FPS
        int                 mDrawCount;             // dx or opengl draw call count per frame
        int                 mTriangleCount;         // dx or opengl triangle count per frame
        IScene*             mActiveScene;           // current active scene
        
        IRender*            mRender;                // renderer object, singleton variant
        LogPrinter*         mLogPrinter;            // log printer, singleton variant
        SceneManager*       mSceneManager;          // scene manager, singleton variant
        ResourceManager*    mResourceMgr;           // resource manager, singleton variant
        EventDispatcher*    mEventDispatcher;       // event dispatcher, such as touch/keyboard/mouse. singleton variant
        LayoutManager*      mLayoutManager;         // layout manager, use to load UI xml. singleton variant
        
        RenderFeatureLevel  mFeatureLevel;          // render feature level, OpenGL3 and OpenGL2 use diff GLSL
        LanguageType        mLocalLanguage;         // save local language type, set by PlatformXXX.
        std::string         mOSVerStr;              // OS version string
        std::string         mDeviceModel;           // device model, include brand and models
        std::string         mWriteablePath;         // platform writeable path
        
        std::list<Scheduler*>       mSchedulerList;     // all scheduler
        std::vector<Scheduler*>     mAddSchedulerList;  // need added scheduler in next loop
        PlatformCreationParams      mCreationParams;// peach3d init params
    };
    
    // get render feature level
#define PD_RENDERLEVEL() IPlatform::getSingletonPtr()->getRenderFeatureLevel()
#define PD_RENDERLEVEL_GL3() (IPlatform::getSingletonPtr()->getRenderFeatureLevel() == RenderFeatureLevel::eGL3)
#if PEACH3D_DEBUG == 1
    // add draw call count and triangle count
    #define PD_ADD_DRAWCALL(count) IPlatform::getSingletonPtr()->addDrawCallCount(count)
    #define PD_ADD_DRAWTRIAGNLE(count) IPlatform::getSingletonPtr()->addDrawTriangleCount(count)
#else
    #define PD_ADD_DRAWCALL(count)
    #define PD_ADD_DRAWTRIAGNLE(count)
#endif
}

#endif // PEACH3D_PLATFORM_H
