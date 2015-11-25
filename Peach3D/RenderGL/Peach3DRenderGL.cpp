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
        // delete global AABB buffers
        ObjectGL::deleteAABBBuffers();
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
        std::string renderer = (const char*)glGetString(GL_RENDERER);
        Peach3DLog(LogLevel::eInfo, "Renderer: %s",renderer.c_str());
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
        glViewport(0, 0, int(size.x + 0.5f), int(size.y + 0.5f));
        Peach3DLog(LogLevel::eInfo, "Render window's width %.1f, height %.1f", size.x, size.y);
        
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
    
    void RenderGL::prepareForRender()
    {
        // is need clean stencil
        static bool isStencilEnabled = (IPlatform::getSingleton().getCreationParams().sBits > 0);
        GLbitfield clearBit = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
        if (isStencilEnabled)
        {
            clearBit = clearBit | GL_STENCIL_BUFFER_BIT;
        }
        glClear(clearBit);
    }
    
    void RenderGL::prepareForObjectRender()
    {
        // open depth test
        glEnable(GL_DEPTH_TEST);
    }
    
    void RenderGL::prepareForWidgetRender()
    {
        // close depth test
        glDisable(GL_DEPTH_TEST);
        
        // update widget global uniforms for GL3 and DX
        if (PD_RENDERLEVEL() == RenderFeatureLevel::eGL3) {
            ProgramGL::updateGlobalWidgetUnifroms();
        }
    }
    
    IProgram* RenderGL::createProgram(uint pId)
    {
        ProgramGL* program = new ProgramGL(pId);
        return program;
    }
    
    ITexture* RenderGL::createTexture(const char* name)
    {
        Peach3DAssert(strlen(name)>0, "The texture name can't be null");
        TextureGL* texture = new TextureGL(name);
        return texture;
    }
    
    IObject* RenderGL::createObject(const char* objectName)
    {
        Peach3DAssert(strlen(objectName)>0, "The object name can't be null");
        
        ObjectGL* object = new ObjectGL(objectName);
        return object;
    }
    
    void RenderGL::getObjectPresetVSSource(uint* params, std::string* code, std::vector<ProgramUniform>* uniforms)
    {/*
        uint vertexType = params[0];
        bool scrollEnabled = params[2] < params[1]; // enable uv scroll if scrolltexindex is valid
        std::vector<std::string> codeList;
        RenderFeatureLevel level = IPlatform::getSingleton().getRenderFeatureLevel();
        if(level == RenderFeatureLevel::eGL3)
        {
            // add global uniform block
            codeList.push_back("uniform GlobalUnifroms\n"
                               "{\n"
                               "    mat4 pd_projMatrix;\n"
                               "    mat4 pd_viewMatrix;\n"
                               "    vec4 pd_ambient;\n"
                               "};\n");
            // add object uniform block
            codeList.push_back("uniform ObjectUnifroms\n"
                               "{\n"
                               "    mat4 pd_modelMatrix;\n");
            uniforms->push_back(ProgramUniform(pdShaderModelMatrixUniformName, UniformType::eMatrix4));
            if ((vertexType & VertexTypeUV) && scrollEnabled)
            {
                codeList.push_back("    vec2 pd_scrollUV;\n");
                uniforms->push_back(ProgramUniform(pdShaderScrollUVUniformName, UniformType::eVector2));
            }
            codeList.push_back("};\n");
            // add vertex attri
            codeList.push_back("in vec3 pd_vertex;\n");
            if (vertexType & VertexTypeColor4)
            {
                codeList.push_back("in vec4 pd_color;\n");
            }
            if (vertexType & VertexTypeNormal)
            {
                codeList.push_back("in vec3 pd_normal;\n");
            }
            if (vertexType & VertexTypeUV)
            {
                codeList.push_back("in vec2 pd_uv;\n");
            }
            // add out put uniform
            codeList.push_back("out vec4 out_color;\n");
            if (vertexType & VertexTypeUV)
            {
                codeList.push_back("out vec2 out_uv;\n");
                if (scrollEnabled)
                {
                    codeList.push_back("out vec2 out_scrollUV;\n");
                }
            }
        }
        else
        {
            // add global uniform block
            codeList.push_back("uniform mat4 pd_projMatrix;\n");
            codeList.push_back("uniform mat4 pd_viewMatrix;\n");
            codeList.push_back("uniform vec4 pd_ambient;\n");
            // add object uniform block
            codeList.push_back("uniform mat4 pd_modelMatrix;\n");
            uniforms->push_back(ProgramUniform(pdShaderModelMatrixUniformName, UniformType::eMatrix4));
            // add vertex attri
            codeList.push_back("attribute vec3 pd_vertex;\n");
            if (vertexType & VertexTypeColor4)
            {
                codeList.push_back("attribute vec4 pd_color;\n");
            }
            if (vertexType & VertexTypeNormal)
            {
                codeList.push_back("attribute vec3 pd_normal;\n");
            }
            if (vertexType & VertexTypeUV)
            {
                codeList.push_back("attribute vec2 pd_uv;\n");
            }
            // add out put uniform
            codeList.push_back("varying vec4 out_color;\n");
            if (vertexType & VertexTypeUV)
            {
                codeList.push_back("varying vec2 out_uv;\n");
            }
        }
        // add main func
        codeList.push_back("void main(void)\n");
        codeList.push_back("{\n");
        // out put position
        codeList.push_back("    mat4 mvpMatrix = pd_projMatrix * pd_viewMatrix * pd_modelMatrix;\n");
        codeList.push_back("    gl_Position = mvpMatrix * vec4(pd_vertex, 1.0);\n");
        if (vertexType & VertexTypeColor4)
        {
            codeList.push_back("    out_color = mix(pd_ambient, pd_color, pd_color.a);\n");
        }
        else
        {
            codeList.push_back("    out_color = pd_ambient;\n");
        }
        if (vertexType & VertexTypeUV)
        {
            if (scrollEnabled && level == RenderFeatureLevel::eGL3)
            {
                codeList.push_back("    out_scrollUV = pd_scrollUV;\n");
            }
            codeList.push_back("    out_uv = pd_uv;\n");
        }
        // add main end
        codeList.push_back("}\n");
        std::for_each(codeList.begin(), codeList.end(), [&](const std::string &piece){ *code += piece; });
        Peach3DLog(LogLevel::eInfo, "vertex code: \n%s", code->c_str());*/
    }
    
    void RenderGL::getObjectPresetPSSource(uint* params, std::string* code, std::vector<ProgramUniform>* uniforms)
    {/*
        uint vertexType = params[0];
        uint texCount = params[1];
        bool scrollEnabled = params[2] < params[1]; // enable uv scroll if scrolltexindex is valid
        std::vector<std::string> codeList;
        RenderFeatureLevel level = IPlatform::getSingleton().getRenderFeatureLevel();
        if(level == RenderFeatureLevel::eGL3)
        {
            codeList.push_back("in vec4 out_color;\n");
            if (vertexType & VertexTypeUV)
            {
                codeList.push_back("in vec2 out_uv;\n");
                if (scrollEnabled)
                {
                    codeList.push_back("in vec2 out_scrollUV;\n");
                }
            }
            codeList.push_back("out vec4 out_FragColor;\n");
        }
        else
        {
            codeList.push_back("varying vec4 out_color;\n");
            if (vertexType & VertexTypeUV)
            {
                codeList.push_back("varying vec2 out_uv;\n");
                // GLES2 can put uniform in pixel shader
                if (scrollEnabled)
                {
                    codeList.push_back("uniform vec2 pd_scrollUV;\n");
                    uniforms->push_back(ProgramUniform(pdShaderScrollUVUniformName, UniformType::eVector2));
                }
            }
        }
        // add texture sampler
        const char* texNames[]={pdShaderTexture0UniformName, pdShaderTexture1UniformName,
            pdShaderTexture2UniformName, pdShaderTexture3UniformName};
        for (auto i=0; i<texCount; ++i)
        {
            codeList.push_back("uniform sampler2D ");
            codeList.push_back(texNames[i]);
            codeList.push_back(";\n");
        }
        // add main func
        codeList.push_back("void main(void)\n");
        codeList.push_back("{\n");
        if (texCount > 0 && (vertexType & VertexTypeUV))
        {
            if (scrollEnabled)
            {
                SHADER_PUSH_LEVEL_SOURCE("    vec2 input_uv = out_uv + out_scrollUV;\n", "    vec2 input_uv = out_uv + pd_scrollUV;\n")
                SHADER_CLAMP_UNIFORM_0_1(input_uv.x)
                SHADER_CLAMP_UNIFORM_0_1(input_uv.y)
            }
            
            codeList.push_back("    vec4 input_color = out_color;\n");
            for (auto i=0; i<texCount; ++i)
            {
                // use orign uv if current is not scrolled texture
                std::string uvName = params[2] == i ? "input_uv" : "out_uv";
                char lineCode[100] = {};
                if (level == RenderFeatureLevel::eGL3)
                {
                    sprintf(lineCode, "    vec4 tex_color%d = texture( %s, %s );\n", i, texNames[i], uvName.c_str());
                }
                else
                {
                    sprintf(lineCode, "    vec4 tex_color%d = texture2D( %s, %s );\n", i, texNames[i], uvName.c_str());
                }
                codeList.push_back(lineCode);
                sprintf(lineCode, "    input_color = mix(input_color, tex_color%d, tex_color%d.a);\n", i, i);
                codeList.push_back(lineCode);
            }
            SHADER_PUSH_LEVEL_SOURCE("    out_FragColor = input_color;\n", "    gl_FragColor = input_color;\n")
        }
        else {
            SHADER_PUSH_LEVEL_SOURCE("    out_FragColor = out_color;\n", "    gl_FragColor = out_color;\n")
        }
        // add main end
        codeList.push_back("}\n");
        std::for_each(codeList.begin(), codeList.end(), [&](const std::string &piece){ *code += piece; });
        Peach3DLog(LogLevel::eInfo, "pixel code: \n%s", code->c_str());*/
    }
}
