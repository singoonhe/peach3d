//
//  Peach3DPlatformAndroid.cpp
//  Peach3DLib
//
//  Created by singoon he on 14-9-1.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#include <android/bitmap.h>
#include <sys/stat.h>
#include <sys/system_properties.h>
#include "Peach3DUtils.h"
#include "Peach3DPlatformAndroid.h"
#include "Peach3DRenderGL.h"

// label texture
std::map<std::string, std::vector<Peach3D::Rect>> labelClicksRect;

extern "C" {
/*
 * Class:     com_peach3d_lib_Peach3DActivity
 * Method:    nativeCreateBitmapFromFile
 * Signature: (Ljava/lang/String;)Lcom/peach3d/lib/Peach3DActivity/Bitmap;
 */
JNIEXPORT jobject JNICALL Java_com_peach3d_lib_Peach3DActivity_nativeCreateBitmapFromFile
  (JNIEnv *env, jobject obj, jstring file)
  {
    jobject texBitmap = nullptr;
    ulong texLength = 0;
    // get texture file data
    const char *fileStr = env->GetStringUTFChars(file, NULL);
    uchar *texData = Peach3D::ResourceManager::getSingleton().getFileData(fileStr, &texLength);
    if (texLength > 0 && texData) {
        jclass bitmapfactory_class=nullptr;
        do {
            bitmapfactory_class = (jclass)env->FindClass("android/graphics/BitmapFactory");
            if (!bitmapfactory_class) {
                Peach3D::Peach3DErrorLog("Find class \"android/graphics/BitmapFactory\" failed!");
                break;
            }
            jmethodID mid = env->GetStaticMethodID(bitmapfactory_class, "decodeByteArray", "([BII)Landroid/graphics/Bitmap;");
            if (!mid) {
                Peach3D::Peach3DErrorLog("Get static method \"decodeByteArray\" failed!");
                break;
            }
            jbyteArray pixelArray = env->NewByteArray(texLength);
            env->SetByteArrayRegion(pixelArray, 0, texLength, (const signed char*)texData);
            texBitmap = env->CallStaticObjectMethod(bitmapfactory_class, mid, pixelArray, 0, texLength);
            env->DeleteLocalRef(pixelArray);
        }while(0);

        // release memory data
        if (bitmapfactory_class) {
            env->DeleteLocalRef(bitmapfactory_class);
        }
        free(texData);
    }
    env->ReleaseStringUTFChars(file, fileStr);
    return texBitmap;
  }
/*
 * Class:     com_peach3d_lib_Peach3DActivity
 * Method:    nativeAddLabelClickRect
 * Signature: (Ljava/lang/String;IIII)V
 */
JNIEXPORT void JNICALL Java_com_peach3d_lib_Peach3DActivity_nativeAddLabelClickRect
  (JNIEnv *env, jobject obj, jstring key, jint startX, jint startY, jint width, jint height)
  {
    const char *keyStr = env->GetStringUTFChars(key, NULL);
    // add clicked rect
    labelClicksRect[keyStr].push_back(Peach3D::Rect(startX, startY, width, height));
    env->ReleaseStringUTFChars(key, keyStr);
  }
}

namespace Peach3D
{
    PlatformAndroid::~PlatformAndroid()
    {
        // render will auto delete in IPlatform, so do nothing here
    }

    void PlatformAndroid::deleteRenderDependency()
    {
        // delete EGL
        if (mDisplay != EGL_NO_DISPLAY) {
            eglMakeCurrent(mDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            if (mContext != EGL_NO_CONTEXT) {
                eglDestroyContext(mDisplay, mContext);
            }
            if (mSurface != EGL_NO_SURFACE)  {
                eglDestroySurface(mDisplay, mSurface);
            }
            eglTerminate(mDisplay);
        }
        mDisplay = EGL_NO_DISPLAY;
        mContext = EGL_NO_CONTEXT;
        mSurface = EGL_NO_SURFACE;
    }

    bool PlatformAndroid::initWithParams(const PlatformCreationParams &params, ANativeActivity* activity, AConfiguration* config)
    {
        // get system language
        char lang[2], country[2];
        AConfiguration_getLanguage(config, lang);
        AConfiguration_getCountry(config, country);
        if (strncmp(lang, "zh", 2) == 0) {
            if (strncmp(country, "CN", 2) == 0) {
                mLocalLanguage = LanguageType::eChineseHans;
            }
            else {
                mLocalLanguage = LanguageType::eChineseHant;
            }
        }
        else if (strncmp(lang, "en", 2) == 0) {
            mLocalLanguage = LanguageType::eEnglish;
        }
        else if (strncmp(lang, "fr", 2) == 0) {
            mLocalLanguage = LanguageType::eFrench;
        }
        else if (strncmp(lang, "ru", 2) == 0) {
            mLocalLanguage = LanguageType::eRussian;
        };

        // get OS version string
        int32_t version = AConfiguration_getSdkVersion(config);
        char verStr[10] = {0};
        sprintf(verStr, "%d", version);
        mOSVerStr = verStr;

        // get device model
        char man[PROP_VALUE_MAX + 1], mod[PROP_VALUE_MAX + 1];
        /* A length 0 value indicates that the property is not defined */
        int lman = __system_property_get("ro.product.manufacturer", man);
        int lmod = __system_property_get("ro.product.model", mod);
        if ((lman + lmod) > 0) {
            mDeviceModel = Utils::formatString("%s (%s)", man, mod);
        }

        // get writeable path, external path first
        if (activity->externalDataPath) {
            mWriteablePath = activity->externalDataPath;
        }
        else {
            mWriteablePath = activity->internalDataPath;
        }
        if (mWriteablePath[mWriteablePath.size() - 1] != '/') {
            mWriteablePath = mWriteablePath + "/";
        }
        // mkdir
        struct stat sb;
        int32_t res = stat(mWriteablePath.c_str(), &sb);
        if (0 != res && !(sb.st_mode & S_IFDIR)) {
            size_t findPos = mWriteablePath.find('/', strlen("/mnt/"));
            // android have no "mkdirs", so regret
            while(findPos != std::string::npos && findPos < mWriteablePath.size()) {
                std::string dirPath = mWriteablePath.substr(0, findPos+1);
                res = mkdir(dirPath.c_str(), 0777);
                findPos = mWriteablePath.find('/', findPos+1);
            }
        }

        // init base Platform
        IPlatform::initWithParams(params);

        // save activity and vm, this will used for read file
        mActivity = activity;
        mJavaVM = activity->vm;
        mEnv = activity->env;
        mActivityObject = activity->clazz;

        // cache find class jni method
        callJniFunc([&](JNIEnv* env, jclass activityClazz) {
            auto classLoaderClass = env->FindClass("java/lang/ClassLoader");
            auto getClassLoaderMethod = env->GetMethodID(activityClazz, "getClassLoader",
                                                        "()Ljava/lang/ClassLoader;");
            auto loader = env->CallObjectMethod(this->mActivityObject, getClassLoaderMethod);
            // class loader need global ref.
            this->mClassLoader = env->NewGlobalRef(loader);
            this->mFindClassMethod = env->GetMethodID(classLoaderClass, "findClass",
                                                    "(Ljava/lang/String;)Ljava/lang/Class;");
        });

        // check init params
        if (mCreationParams.MSAA > 1) {
            mCreationParams.MSAA = 1;
            Peach3DLog(LogLevel::eWarn, "Android only support 1 or 0 MSAA, %d not support", mCreationParams.MSAA);
        }

        // add default search file dir
        ResourceManager::getSingleton().setAssetsManager(activity->assetManager);
        ResourceManager::getSingleton().addSearchDirectory("assets/");
        return true;
    }

    void PlatformAndroid::setAndroidWindow(void* window)
    {
        void* oldWindow = mCreationParams.window;
        IPlatform::setWindow(window);

        if (!oldWindow) {
            // init display
            mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
            eglInitialize(mDisplay, 0, 0);

            // initialize EGL config with OpenGL ES 3.0
            EGLint attribs[] = {
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, // OpenGL ES 2.0. This must be set, if not some device     using OpenGL ES-CM 1.1 will report error "called unimplemented OpenGL ES API"
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_BLUE_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_RED_SIZE, 8,
                EGL_DEPTH_SIZE, mCreationParams.zBits,
                EGL_STENCIL_SIZE, mCreationParams.sBits,
                EGL_SAMPLE_BUFFERS, (mCreationParams.MSAA > 0) ? 1 : 0,
                EGL_SAMPLES, mCreationParams.MSAA,
                EGL_NONE
            };
            EGLint numConfigs;
            eglChooseConfig(mDisplay, attribs, &mEGLConfig, 1, &numConfigs);

            // set render android native window
            eglGetConfigAttrib(mDisplay, mEGLConfig, EGL_NATIVE_VISUAL_ID, &mEGLFormat);
        }
        ANativeWindow_setBuffersGeometry((ANativeWindow*)window, 0, 0, mEGLFormat);
        // create surface, destroy old surface
        if (mSurface != EGL_NO_SURFACE) {
            eglDestroySurface(mDisplay, mSurface);
            mSurface = EGL_NO_SURFACE;
        }
        mSurface = eglCreateWindowSurface(mDisplay, mEGLConfig, (ANativeWindow*)window, NULL);
        if (!oldWindow) {
            // set egl context client version 2. OpenGL ES CM 1.1 will use if not set.
            EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
            mContext = eglCreateContext(mDisplay, mEGLConfig, NULL, context_attribs);
            if (mContext == EGL_NO_CONTEXT) {
                // use OpenGL ES CM 1.1 if version 2 failed
                mContext = eglCreateContext(mDisplay, mEGLConfig, NULL, NULL);
            }
        }
        if (eglMakeCurrent(mDisplay, mSurface, mSurface, mContext) == EGL_FALSE) {
            Peach3DLog(LogLevel::eError, "Unable to eglMakeCurrent");
            return ;
        }
        if (!oldWindow) {
            EGLint width, height;
            // save created window size
            eglQuerySurface(mDisplay, mSurface, EGL_WIDTH, &width);
            eglQuerySurface(mDisplay, mSurface, EGL_HEIGHT, &height);
            mCreationParams.winSize = Peach3D::Vector2(width, height);

            // create Render
            mRender = new RenderGL();
            // at last, init render after get final window size
            bool success = mRender->initRender(mCreationParams.winSize);
            if (success) {
                // check which GL version should use, add support extensions
                addExtensionsSupport(mRender);

                // notify IAppDelegate launch finished
                if (mCreationParams.delegate->appDidFinishLaunching()) {
                    // start animating
                    mAnimating = true;
                }
            }
        }
    }

    void PlatformAndroid::addExtensionsSupport(IRender* render)
    {
        RenderGL* glRender = (RenderGL*)render;
        // check opengl es which version used
        if (glRender->isTypeExtersionSupport(GLExtensionType::eAndroidGL3)) {
            // save OpenGL ES version 3
            mFeatureLevel = RenderFeatureLevel::eGL3;
            Peach3DLog(LogLevel::eInfo, "Render feature level GL3 be used");
        }
        else {
            // save OpenGL ES version 2
            mFeatureLevel = RenderFeatureLevel::eGL2;
            Peach3DLog(LogLevel::eInfo, "Render feature level GL2 be used");

#ifdef ANDROID_DYNAMIC_ES3
            // add VAO func for extension
            if (glRender->isTypeExtersionSupport(GLExtensionType::eVertexArray)) {
                glGenVertexArrays = (PFNGLGENVERTEXARRAYSOESPROC)eglGetProcAddress ( "glGenVertexArraysOES" );
                glBindVertexArray = (PFNGLBINDVERTEXARRAYOESPROC)eglGetProcAddress ( "glBindVertexArrayOES" );
                glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSOESPROC)eglGetProcAddress ( "glDeleteVertexArraysOES" );
                glIsVertexArray = (PFNGLISVERTEXARRAYOESPROC)eglGetProcAddress ( "glIsVertexArrayOES" );

                if (!glGenVertexArrays || !glBindVertexArray || !glDeleteVertexArrays || !glIsVertexArray) {
                    // can't get adress, extersion is not supported
                    glRender->deleteExtersionSupport(GLExtensionType::eVertexArray);
                }
                else {
                    Peach3DLog(LogLevel::eInfo, "GL extension vertex_array_object is supported");
                }
            }
            // add map buffer func for extension
            if (glRender->isTypeExtersionSupport(GLExtensionType::eMapBuffer)) {
                glMapBufferOES = (PFNGLMAPBUFFEROESPROC)eglGetProcAddress ( "glMapBufferOES" );
                glUnmapBufferOES = (PFNGLUNMAPBUFFEROESPROC)eglGetProcAddress ( "glUnmapBufferOES" );

                if (!glMapBufferOES || !glUnmapBufferOES) {
                    // can't get adress, extersion is not supported
                    glRender->deleteExtersionSupport(GLExtensionType::eMapBuffer);
                }
                else {
                    Peach3DLog(LogLevel::eInfo, "GL extension mapbuffer is supported");
                }
            }
#endif
        }
    }

    void PlatformAndroid::renderOneFrame(float lastFrameTime)
    {
        // calculate the fixed frame delay time
        static const long fixedFrameTime = 1000000000/mCreationParams.maxFPS;
        static const long multiFFT = 10 * fixedFrameTime;

        if  (mAnimating && isRenderWindowValid()) {
            // get current time
            timespec now;
            clock_gettime(CLOCK_MONOTONIC, &now);
            uint64_t nowNs = now.tv_sec * 1000000000ull + now.tv_nsec;
            if (mLastTime > 0)  {
        		static uint64_t fpsTime = 0;
                fpsTime += nowNs - mLastTime;

                if (fpsTime >= multiFFT) {
                    // render one frame with time
                    IPlatform::renderOneFrame(fpsTime * 0.000000001f);
                    // swap buffers
                    eglSwapBuffers(mDisplay, mSurface);

                    fpsTime = 0;
                }
                else if (fpsTime >= fixedFrameTime) {
                    // render one frame with time
                    IPlatform::renderOneFrame(fixedFrameTime * 0.000000001f);
                    // swap buffers
                    eglSwapBuffers(mDisplay, mSurface);
                    fpsTime -= fixedFrameTime;
                }
            }

            // save current time
            mLastTime = nowNs;
        }
    }

    void PlatformAndroid::resumeAnimating()
    {
        IPlatform::resumeAnimating();
        // reset last time
        mLastTime = 0;
    }

    void PlatformAndroid::terminate()
    {
        IPlatform::terminate();
        // delete activity
        ANativeActivity_finish(mActivity);
    }

    void PlatformAndroid::callJniFunc(const std::function<void(JNIEnv*, jclass)>& callback, const char* className)
    {
        JNIEnv *env;
        int status = mJavaVM->GetEnv((void**)&env, JNI_VERSION_1_6);
        do {
            if(status < 0) {
                status = mJavaVM->AttachCurrentThread(&env, NULL);
                if(status < 0) {
                    Peach3DErrorLog("Get Jni env failed!");
                    break;
                }
            }

            // get class
            jclass findClazz = nullptr;
            if (className && strlen(className) > 0) {
                jstring classStr = env->NewStringUTF(className);
                findClazz = static_cast<jclass>(env->CallObjectMethod(mClassLoader, mFindClassMethod, classStr));
                env->DeleteLocalRef(classStr);
            }
            else {
                findClazz = env->GetObjectClass(mActivityObject);
            }
            if (!findClazz) {
                Peach3DErrorLog("Get class \"%s\" from jni failed!", className);
                break;
            }

            // callback func
            callback(env, findClazz);
            // detach thread.
            mJavaVM->DetachCurrentThread();
        } while(0);
    }

    ITexture* PlatformAndroid::getTextTexture(const std::vector<LabelStageTextInfo>& textList, const LabelTextDefined& defined,
                                          std::map<std::string, std::vector<Rect>>& clicksRect)
    {
        ITexture* texture = nullptr;
        labelClicksRect.clear();

        callJniFunc([&](JNIEnv* env, jclass activityClazz) {
            jobject bitmap = nullptr;
            do {
                jmethodID bitmapMethodID = env->GetStaticMethodID(activityClazz, "createTextBitmap",
                    "([Ljava/lang/String;[Ljava/lang/String;[I[ZLjava/lang/String;IIIII)Landroid/graphics/Bitmap;");
                if (!bitmapMethodID) {
                    Peach3DErrorLog("Get static method \"createTextBitmap\" failed!");
                    break;
                }

                // generate params
                jclass jclsStr = env->FindClass("java/lang/String");
                jobjectArray textArrayList = env->NewObjectArray(textList.size(), jclsStr, 0);
                jobjectArray imageArrayList = env->NewObjectArray(textList.size(), jclsStr, 0);
                jintArray colorArrayList = env->NewIntArray(textList.size());
                jbooleanArray clickArrayList = env->NewBooleanArray(textList.size());
                env->DeleteLocalRef(jclsStr);
                for (int i=0; i<textList.size(); i++) {
                    // save text list
                    jstring textString = env->NewStringUTF(textList[i].text.c_str());
                    env->SetObjectArrayElement(textArrayList, i, textString);
                    env->DeleteLocalRef(textString);
                    // save image list
                    jstring imageString = env->NewStringUTF(textList[i].image.c_str());
                    env->SetObjectArrayElement(imageArrayList, i, imageString);
                    env->DeleteLocalRef(imageString);
                    // color list
                    uint curColor=0;
                    curColor += uint(textList[i].color.a*255) << 24;
                    curColor += uint(textList[i].color.r*255) << 16;
                    curColor += uint(textList[i].color.g*255) << 8;
                    curColor += uint(textList[i].color.b*255);
                    env->SetIntArrayRegion(colorArrayList, i, 1, (int*)(&curColor));
                    // click enabled list
                    jboolean isClickEnabled = textList[i].clickEnabled;
                    env->SetBooleanArrayRegion(clickArrayList, i, 1, &isClickEnabled);
                }
                // font name
                jstring jstrText = env->NewStringUTF(defined.font.c_str());
                // alignment
                int totalAlign = (int)defined.imageVAlign;
                totalAlign = (totalAlign << 4) | (int)defined.vAlign;
                totalAlign = (totalAlign << 4) | (int)defined.hAlign;
                // call function
                bitmap = env->CallStaticObjectMethod(activityClazz, bitmapMethodID, textArrayList, imageArrayList,  colorArrayList, clickArrayList, jstrText, (int)defined.fontSize, totalAlign, (int)defined.dim.x, (int)defined.dim.y, (int)mCreationParams.winSize.x);
                env->DeleteLocalRef(textArrayList);
                env->DeleteLocalRef(imageArrayList);
                env->DeleteLocalRef(colorArrayList);
                env->DeleteLocalRef(clickArrayList);
                env->DeleteLocalRef(jstrText);

                // create texture from android bitmap
                AndroidBitmapInfo bmpInfo={0};
                if(AndroidBitmap_getInfo(env, bitmap, &bmpInfo)<0) {
                    Peach3DErrorLog("AndroidBitmap_getInfo failed!");
                    break;
                }
                uchar* dataFromBmp=nullptr;
                if(AndroidBitmap_lockPixels(env, bitmap, (void**)&dataFromBmp)) {
                    Peach3DErrorLog("AndroidBitmap_lockPixels failed!");
                    break;
                }
                int bufSize = bmpInfo.stride*bmpInfo.height;
                texture = ResourceManager::getSingleton().createTexture(dataFromBmp, bufSize, bmpInfo.width, bmpInfo.   height, TextureFormat::eRGBA8);
                AndroidBitmap_unlockPixels(env, bitmap);
            } while (0);

            // release bitmap
            if (bitmap) {
                env->DeleteLocalRef(bitmap);
            }
        });

        // return click rects
        clicksRect = labelClicksRect;
        return texture;
    }

    void PlatformAndroid::openUrl(const std::string& url)
    {
        if (url.size()==0) return;

        callJniFunc([&](JNIEnv* env, jclass activityClazz) {
            // auto add "http://" header
            std::string finalUrl = url;
            if (finalUrl.find("http://")==std::string::npos) {
                finalUrl = "http://" + finalUrl;
            }
            jmethodID urlMethodID = env->GetStaticMethodID(activityClazz, "openUrl", "(Ljava/lang/String;)V");
            if (!urlMethodID) {
                Peach3DErrorLog("Get static method \"openUrl\" failed!");
                return;
            }
            jstring jstrText = env->NewStringUTF(finalUrl.c_str());
            env->CallStaticVoidMethod(activityClazz, urlMethodID, jstrText);
            env->DeleteLocalRef(jstrText);
        });
    }
}