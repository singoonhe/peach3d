//
//  Peach3DPlatformAndroid.h
//  Peach3DLib
//
//  Created by singoon he on 14-9-1.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#ifndef PEACH3D_PLATFORMANDROID_H
#define PEACH3D_PLATFORMANDROID_H

#include <EGL/egl.h>
#include <android/native_activity.h>
#include <android/configuration.h>
#include "Peach3DIPlatform.h"
#include "Peach3DCommonGL.h"
#include "Peach3DResourceManager.h"

namespace Peach3D
{
    class PlatformAndroid : public IPlatform
    {
    public:
        PlatformAndroid() : IPlatform(), mDisplay(EGL_NO_DISPLAY), mSurface(EGL_NO_CONTEXT), mContext(EGL_NO_SURFACE), mLastTime(0), mEGLFormat(0) {}
        ~PlatformAndroid();
        // init windows platform, using AConfiguration to get local language.
        virtual bool initWithParams(const PlatformCreationParams &params, ANativeActivity* activity, AConfiguration* config);
        // set window handler, init rendergl
        virtual void setAndroidWindow(void* window);
        //! calc delta time, reduce the times for render frame
        virtual void renderOneFrame(float lastFrameTime);

        /** Deal with input event. */
        int32_t onInputEvent(AInputEvent* event);
        //! is application terminate, it's needed to trigger exit in android
        virtual bool isTerminating() { return mTerminating; }
        //! is application animating, android render frame if animating
        virtual bool isAnimating() { return mAnimating; }
        //! resume animating, reset last time
        virtual void resumeAnimating();
        /** Terminate application, finish activity. */
        virtual void terminate();

        /** Return JNIEnv. */
        JNIEnv* getEnv() { return mEnv; }
        jobject getActivityObject() { return mActivityObject; }
        /** Call jni func, activity class will use if className not set. */
        void callJniFunc(const std::function<void(JNIEnv*, jclass)>& callback, const char* className = nullptr);
        //! get label text texture
        TexturePtr getTextTexture(const std::vector<LabelStageTextInfo>& textList, const LabelTextDefined& defined,
                                 std::map<std::string, std::vector<Rect>>& clicksRect);
        /** Open url, "http://" is not necessary. */
        virtual void openUrl(const std::string& url);

    protected:
        /** Add extensions support, set render feature level */
        void addExtensionsSupport(IRender* render);

    private:
        EGLDisplay mDisplay;
        EGLSurface mSurface;
        EGLContext mContext;
        EGLint     mEGLFormat;
        EGLConfig  mEGLConfig;

        JavaVM*             mJavaVM;
        JNIEnv*             mEnv;
        jobject             mActivityObject;
        jobject             mClassLoader;
        jmethodID           mFindClassMethod;
        ANativeActivity*    mActivity;
        uint64_t            mLastTime;
    };
}

#endif // PEACH3D_PLATFORMANDROID_H