//
//  RenderGL.m
//  RenderGL
//
//  Created by singoon he on 12-4-15.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#include "Peach3DRenderGL.h"
#include "Peach3DLogPrinter.h"
#include "Peach3DProgramGL.h"
#include "Peach3DObjectGL.h"
#include "Peach3DResourceManager.h"
#include "Peach3DTextureGL.h"
#include "Peach3DIPlatform.h"

namespace Peach3D
{
    RenderGL::~RenderGL()
    {
        // delete object global UBO
        ProgramGL::deleteGlobalUBO();
    }
    
    bool RenderGL::initRender(const Vector2& size)
    {
        // call base init
        IRender::initRender(size);
        
        // renderer info
        std::string vendor = (const char*)glGetString(GL_VENDOR);
        Peach3DLog(LogLevel::eInfo, "Verdor: %s",vendor.c_str());
        mVideoCard = (const char*)glGetString(GL_RENDERER);
        Peach3DLog(LogLevel::eInfo, "Renderer: %s",mVideoCard.c_str());
        std::string version = (const char*)glGetString(GL_VERSION);
        Peach3DLog(LogLevel::eInfo, "Version: %s",version.c_str());
        // get support 
        const char* extensions = (const char*)glGetString(GL_EXTENSIONS);
        filterGLExtensions(version.c_str(), extensions);
        /* Peach3DLog(LogLevel::eInfo, "Extensions: %s", extensions); */

        // init render state
        glEnable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glViewport(0, 0, (int)lroundf(size.x), (int)lroundf(size.y));
        Peach3DLog(LogLevel::eInfo, "Render window's width %.0f, height %.0f", size.x, size.y);
        
        return true;
    }

    void RenderGL::filterGLExtensions(const char* version, const char* extension)
    {
#if PEACH3D_CURRENT_PLATFORM == PEACH3D_PLATFORM_ANDROID
        bool gl3FuncExist = true;
#ifdef ANDROID_DYNAMIC_ES3
        gl3FuncExist = gl3stubInit();
#endif
        if (strstr(version, "OpenGL ES 3.") && gl3FuncExist) {
            // support OpenGL ES 3.0 in Android
            mExtensionList.push_back(GLExtensionType::eAndroidGL3);
        }
#endif
        if (extension) {
            // check extensions whick support
            if (strstr(extension, "vertex_array_object")) {
                mExtensionList.push_back(GLExtensionType::eVertexArray);
                // android need judge adress valied
#if PEACH3D_CURRENT_PLATFORM != PEACH3D_PLATFORM_ANDROID
                Peach3DLog(LogLevel::eInfo, "GL extension vertex_array_object is supported");
#endif
            }
            if (strstr(extension, "mapbuffer")) {
                mExtensionList.push_back(GLExtensionType::eMapBuffer);
                // android need judge adress valied
#if PEACH3D_CURRENT_PLATFORM != PEACH3D_PLATFORM_ANDROID
                Peach3DLog(LogLevel::eInfo, "GL extension mapbuffer is supported");
#endif
            }
        }
        
        // set mac support two extension
#if PEACH3D_CURRENT_PLATFORM == PEACH3D_PLATFORM_MAC
        mExtensionList.push_back(GLExtensionType::eVertexArray);
        mExtensionList.push_back(GLExtensionType::eMapBuffer);
#endif
    }
    
    void RenderGL::setRenderClearColor(const Color4& color)
    {
        IRender::setRenderClearColor(color);
        // modify opengl clear color
        glClearColor(mRenderClearColor.r, mRenderClearColor.g, mRenderClearColor.b, mRenderClearColor.a);
    }
    
    void RenderGL::setRenderLineWidth(float newWidth)
    {
        IRender::setRenderLineWidth(newWidth);
        // modify opengl lines width
        glLineWidth(newWidth);
    }
    
    void RenderGL::prepareForMainRender()
    {
        // is need clean stencil
        static bool isStencilEnabled = (IPlatform::getSingleton().getCreationParams().sBits > 0);
        GLbitfield clearBit = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
        if (isStencilEnabled) {
            clearBit = clearBit | GL_STENCIL_BUFFER_BIT;
        }
        glClear(clearBit);
    }
    
    void RenderGL::prepareForObjectRender()
    {
        // open depth test
        glEnable(GL_DEPTH_TEST);
        
        // update object global uniforms for GL3
        if (PD_RENDERLEVEL_GL3()) {
            ProgramGL::updateGlobalObjectUnifroms();
        }
    }
    
    void RenderGL::prepareForWidgetRender()
    {
        // close depth test
        glDisable(GL_DEPTH_TEST);
        
        // update widget global uniforms for GL3
        if (PD_RENDERLEVEL_GL3()) {
            ProgramGL::updateGlobalWidgetUnifroms();
        }
    }
    
    IProgram* RenderGL::createProgram(uint pId)
    {
        ProgramGL* program = new ProgramGL(pId);
        return program;
    }
    
    TexturePtr RenderGL::createTexture(const char* name)
    {
        Peach3DAssert(strlen(name)>0, "The texture name can't be null");
        TexturePtr texture(new TextureGL(name));
        return texture;
    }
    
    ObjectPtr RenderGL::createObject(const char* objectName)
    {
        Peach3DAssert(strlen(objectName)>0, "The object name can't be null");
        ObjectPtr object(new ObjectGL(objectName));
        return object;
    }
}
