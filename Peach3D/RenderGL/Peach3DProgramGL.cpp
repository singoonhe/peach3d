//
//  Peach3DProgramGL.cpp
//  TestPeach3D
//
//  Created by singoon he on 12-5-17.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#include "Peach3DProgramGL.h"
#include "Peach3DLogPrinter.h"
#include "Peach3DIPlatform.h"
#include "Peach3DVector4.h"
#include "Peach3DIObject.h"
#include "Peach3DRenderGL.h"
#include "Peach3DTextureGL.h"
#include "Peach3DSprite.h"
#include "Peach3DUtils.h"
#include "Peach3DOBB.h"
#include "Peach3DTerrain.h"
#include "Peach3DLayoutManager.h"
#include "Peach3DResourceManager.h"
#include "Peach3DSceneManager.h"

namespace Peach3D
{
    // patch widget count each auto increase
    #define INSTANCED_COUNT_INCREASE_STEP   50
    // define global UBO binding point
    #define GLOBAL_UBO_BINDING_POINT        0
    // define lights UBO binding point
    #define LIGHTS_UBO_BINDING_POINT        1
    // define shadow UBO binding point
    #define SHADOW_UBO_BINDING_POINT        2
    // define bones UBO binding point
    #define BONES_UBO_BINDING_POINT         3
    
    // defined widget global UBO info
    GLuint ProgramGL::mWidgetUBOId = GL_INVALID_INDEX;
    GLint ProgramGL::mWidgetUBOSize = 0;
    std::vector<ProgramUniform>  ProgramGL::mWidgetUBOUniforms = {ProgramUniform("pd_viewRect", UniformDataType::eVector4)};
    // defined object global UBO info
    GLuint ProgramGL::mObjectUBOId = GL_INVALID_INDEX;
    GLint ProgramGL::mObjectUBOSize = 0;
    std::vector<ProgramUniform>  ProgramGL::mObjectUBOUniforms = {ProgramUniform("pd_projMatrix", UniformDataType::eMatrix4),
        ProgramUniform("pd_viewMatrix", UniformDataType::eMatrix4)};
    
    bool ProgramGL::setVertexShader(const char* data, int size, bool isCompiled)
    {
        Peach3DAssert(mVertexType, "Vertex type need valid before setVertexShader");
        // compile shader, replace data if mVSShader exist
        mVSShader = loadShaderFromMemory(GL_VERTEX_SHADER, data, size);
        if (mVSShader && mPSShader && mVertexType) {
            compileProgram();
        }
        return (mVSShader > 0);
    }

    bool ProgramGL::setPixelShader(const char* data, int size, bool isCompiled)
    {
        Peach3DAssert(mVertexType, "Vertex type need valid before setPixelShader");
        // compile shader
        mPSShader = loadShaderFromMemory(GL_FRAGMENT_SHADER, data, size);
        if (mPSShader && mVSShader && mVertexType) {
            compileProgram();
        }
        return (mPSShader > 0);
    }
    
    GLuint ProgramGL::loadShaderFromMemory(GLenum type, const char* data, int size)
    {
        GLuint shader = 0;
        if ((type==GL_VERTEX_SHADER && mVSShader == 0) || (type == GL_FRAGMENT_SHADER && mPSShader == 0)) {
            shader = glCreateShader(type);
        }
        // define GL version MACROS, "GL_ES" has set by OpenGL ES.
#if PEACH3D_CURRENT_RENDER == PEACH3D_RENDER_GLES
        // Declare for OpenGL ES 3.0
        const char definedShader3[] = "#version 300 es\n#define PD_LEVEL_GL3\nprecision highp float;\n";
        // Declare for OpenGL ES 2.0, using highp float first.
        const char definedVS2[] = "#version 100\n#define PD_LEVEL_GL2\nprecision highp float;\n";
        const char definedPS2[] = "#version 100\n#define PD_LEVEL_GL2\n"
            "#ifdef GL_FRAGMENT_PRECISION_HIGH\nprecision highp float;\n#else\nprecision mediump float;\n#endif\n";
#else
        // Declare for OpenGL 3。3
        const char definedShader3[] = "#version 330\n#define PD_LEVEL_GL3\n";
        // Declare for OpenGL 2.x,
        const char definedVS2[] = "#version 100\n#define PD_LEVEL_GL2\nprecision highp float;\n";
        const char definedPS2[] = "#version 100\n#define PD_LEVEL_GL2\nprecision highp float;\n";
#endif
        const char* shaderStrings[2];
        GLint shaderStringLengths[2];
        if (mDrawInstance) {
            shaderStrings[0] = definedShader3;
            shaderStringLengths[0] = (GLint)strlen(definedShader3);
        }
        else if (!mDrawInstance && type == GL_VERTEX_SHADER) {
            shaderStrings[0] = definedVS2;
            shaderStringLengths[0] = (GLint)strlen(definedVS2);
        }
        else if (!mDrawInstance && type == GL_FRAGMENT_SHADER) {
            shaderStrings[0] = definedPS2;
            shaderStringLengths[0] = (GLint)strlen(definedPS2);
        }
        else {
            Peach3DLog(LogLevel::eError, "Can't find render feature level, is forget set it in PlatformXXX?");
        }
        shaderStrings[1] = data;
        shaderStringLengths[1] = size;
        // compile shader, replace code if shader exist
        glShaderSource(shader, 2, shaderStrings, shaderStringLengths);
        glCompileShader(shader);
        
        // check the compile state
        GLint compileState;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compileState);
        if (compileState == GL_FALSE) {
            // output error log
            Peach3DLog(LogLevel::eError, "Compile %s shader of program %u failed",
                       type == GL_VERTEX_SHADER ? "vertex" : "fragment", mProgramId);
            
#if PEACH3D_DEBUG == 1
            GLint logLength;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
            if (logLength>0) {
                GLchar log[logLength];
                glGetShaderInfoLog(shader, logLength, &logLength, log);
                // output detail info
                Peach3DLog(LogLevel::eError, "Compile shader error log : %s", log);
            }
#endif
            // compile error, delete shader
            glDeleteShader(shader);
            shader = 0;
            mProgramValid = false;
        }
        return shader;
    }
    
    void ProgramGL::setVertexType(uint type, bool drawInstance)
    {
        IProgram::setVertexType(type, drawInstance);
        if (mDrawInstance) {
            mDrawInstance = PD_RENDERLEVEL_GL3();
            // Point Sprite not used gl3 always
            if (mVertexType & VertexType::PSprite) {
                mDrawInstance = false;
            }
        }
    }
    
    void ProgramGL::bindProgramAttrs()
    {
        const std::vector<VertexAttrInfo>& infoList = ResourceManager::getVertexAttrInfoList();
        for (auto info=infoList.begin(); info!=infoList.end(); ++info) {
            // bind vertex attrs
            if (mVertexType & info->type) {
                glBindAttribLocation(mProgram, static_cast<GLuint>(info->locate), info->name.c_str());
            }
        }
    }
    
    void ProgramGL::compileProgram()
    {
        if (mProgram==0) {
            // create program if not exist
            mProgram = glCreateProgram();
        }
        
        // attach shader
        glAttachShader(mProgram, mVSShader);
        glAttachShader(mProgram, mPSShader);
        // bind attris before compile
        bindProgramAttrs();
        // compile program
        glLinkProgram(mProgram);
        
        // check compile state
        GLint status;
        glGetProgramiv(mProgram, GL_LINK_STATUS, &status);
        if (status == GL_FALSE) {
            // output error log
            Peach3DLog(LogLevel::eError, "Compile program %u failed", mProgramId);
            
#if PEACH3D_DEBUG == 1
            GLint logLength;
            glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &logLength);
            if (logLength > 0) {
                GLchar log[logLength];
                glGetProgramInfoLog(mProgram, logLength, &logLength, log);
                Peach3DLog(LogLevel::eError, "Link program error log : %s", log);
            }
#endif
            
            // coimpile failed, delete program
            glDeleteProgram(mProgram);
            mProgram = 0;
            mProgramValid = false;
        }
        else {
            mProgramValid = true;
            
            // bind gobal uniforms when GL3 support, particle not need
            if (mDrawInstance) {
                bindUniformsBuffer("GlobalUniforms", (mVertexType & VertexType::Point3) ? &mObjectUBOId : &mWidgetUBOId,
                                   (mVertexType & VertexType::Point3) ? &mObjectUBOSize : &mWidgetUBOSize,
                                   (mVertexType & VertexType::Point3) ? &mObjectUBOUniforms : &mWidgetUBOUniforms,
                                   GLOBAL_UBO_BINDING_POINT);
            }
        }
        
        // detach and delete shader after program compile
        glDetachShader(mProgram, mVSShader);
        glDetachShader(mProgram, mPSShader);
    }
    
    void ProgramGL::setProgramUniformsDesc(const std::vector<ProgramUniform>& uniformList)
    {
        Peach3DAssert(mVertexType, "Vertex type need valid before setProgramUniformsDesc");
        IProgram::setProgramUniformsDesc(uniformList);
        if (mDrawInstance && uniformList.size() > 0) {
            // save one instanced uniform buffer size
            mUniformsSize = 0;
            for (auto& uniform : uniformList) {
                mUniformsSize += ShaderCode::getUniformFloatBits(uniform.dType) * sizeof(float);
            }
        }
    }
    
    void ProgramGL::setLightsCount(uint count)
    {
        Peach3DAssert(mVertexType, "Vertex type need valid before setLightsCount");
        if (count > 0 && (mVertexType & VertexType::Point3)) {
            IProgram::setLightsCount(count);
            if (mDrawInstance) {
                // set object lights UBO uniforms
                mLightsUBOUniforms = ShaderCode::mLightUniforms;
                // bind lights UBO
                bindUniformsBuffer("LightsUniforms", &mLightsUBOId, &mLightsUBOSize, &mLightsUBOUniforms, LIGHTS_UBO_BINDING_POINT);
            }
        }
    }
    
    void ProgramGL::setShadowCount(uint count)
    {
        Peach3DAssert(mVertexType, "Vertex type need valid before setShadowCount");
        if (count > 0 && (mVertexType & VertexType::Point3)) {
            IProgram::setShadowCount(count);
            if (mDrawInstance) {
                // set object shadow UBO uniforms
                mShadowUBOUniforms = {ProgramUniform("pd_shadowMatrix", UniformDataType::eMatrix4)};
                // bind shadow UBO
                bindUniformsBuffer("ShadowUniforms", &mShadowUBOId, &mShadowUBOSize, &mShadowUBOUniforms, SHADOW_UBO_BINDING_POINT);
            }
        }
    }
    
    void ProgramGL::setBoneCount(uint count)
    {
        Peach3DAssert(mVertexType, "Vertex type need valid before setBoneCount");
        if (count > 0 && (mVertexType & VertexType::Point3)) {
            IProgram::setBoneCount(count);
            if (mDrawInstance) {
                // set bone lights UBO uniforms
                mBoneUBOUniforms = {ProgramUniform("pd_boneMatrix", UniformDataType::eVector4)};
                // bind bone UBO
                bindUniformsBuffer("BoneUniforms", &mBoneUBOId, &mBoneUBOSize, &mBoneUBOUniforms, BONES_UBO_BINDING_POINT);
            }
        }
    }
    
    void ProgramGL::bindUniformsBuffer(const char* uName, GLuint* UBOId, GLint* UBOSize, std::vector<ProgramUniform>* uniforms, GLint index)
    {
        // get global uniform from program, so every program must include uName
        GLuint globalIndex = glGetUniformBlockIndex(mProgram, uName);
        Peach3DAssert(globalIndex != GL_INVALID_INDEX, "block Uniforms must include in vertex shader");
        
        if (globalIndex != GL_INVALID_INDEX) {
            // create global UBO if it not exist
            if ((*UBOId) == GL_INVALID_INDEX) {
                // get global uniform size, may different on platforms
                glGetActiveUniformBlockiv(mProgram, globalIndex, GL_UNIFORM_BLOCK_DATA_SIZE, UBOSize);
                
                glGenBuffers(1, UBOId);
                glBindBuffer(GL_UNIFORM_BUFFER, *UBOId);
                // for standard, buffer size = sizeof(projMatrix) + sizeof(viewMatrix) + sizeof(ambient)
                glBufferData(GL_UNIFORM_BUFFER, *UBOSize, NULL, GL_DYNAMIC_DRAW);
                
                // get offsets
                auto uniformCount = uniforms->size();
                const GLchar **names = (const GLchar **)malloc(sizeof(const GLchar *) * uniformCount);
                for (auto i=0; i<uniformCount; ++i) {
                    names[i] = (*uniforms)[i].name.c_str();
                }
                GLuint indices[uniformCount];
                glGetUniformIndices(mProgram, (GLsizei)uniformCount, names, indices);
                GLint offset[uniformCount];
                glGetActiveUniformsiv(mProgram, (GLsizei)uniformCount, indices, GL_UNIFORM_OFFSET, offset);
                for (auto i=0; i<uniformCount; ++i) {
                    (*uniforms)[i].offset = offset[i];
                }
                glBindBuffer(GL_UNIFORM_BUFFER, 0);
                free(names);
            }
            // bind global buffer to global uniform, provide global info
            glBindBufferBase(GL_UNIFORM_BUFFER, index, *UBOId);
            glUniformBlockBinding(mProgram, globalIndex, index);
        }
    }
    
    void ProgramGL::updateObjectLightsUniforms(const std::vector<LightPtr>& lights)
    {
        Peach3DAssert(lights.size() == mLightsCount, "Light list must equal to program count!");
        if (mLightsUBOId != GL_INVALID_INDEX && mLightsCount > 0) {
            float lData[4 * SceneManager::getSingleton().getLightMax()];
            glBindBuffer(GL_UNIFORM_BUFFER, mLightsUBOId);
            // map lights buffer and copy memory on GL3
            const size_t varArraySize = sizeof(float) * 4 * lights.size();
            float* data = (float*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, mLightsUBOSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
            for (auto& uniform : mLightsUBOUniforms) {
                switch (ShaderCode::getUniformNameType(uniform.name)) {
                    case UniformNameType::eLightTypeSpot: {
                        for (auto i=0; i<lights.size(); ++i) {
                            lData[i * 4] = (float)lights[i]->getType();
                            auto spotExt = lights[i]->getSpotExtend();
                            lData[i * 4 + 1] = spotExt.x;
                            lData[i * 4 + 2] = spotExt.y;
                            lData[i * 4 + 3] = lights[i]->getShadowTexture() ? 1.f: 0.f;
                        }
                        memcpy(data + uniform.offset/sizeof(float), lData, varArraySize);
                    }
                        break;
                    case UniformNameType::eLightPos: {
                        for (auto i=0; i<lights.size(); ++i) {
                            auto pos = lights[i]->getPosition();
                            lData[i * 4] = pos.x;
                            lData[i * 4 + 1] = pos.y;
                            lData[i * 4 + 2] = pos.z;
                        }
                        memcpy(data + uniform.offset/sizeof(float), lData, varArraySize);
                    }
                        break;
                    case UniformNameType::eLightDir: {
                        for (auto i=0; i<lights.size(); ++i) {
                            auto dir = lights[i]->getDirection();
                            lData[i * 4] = dir.x;
                            lData[i * 4 + 1] = dir.y;
                            lData[i * 4 + 2] = dir.z;
                        }
                        memcpy(data + uniform.offset/sizeof(float), lData, varArraySize);
                    }
                        break;
                    case UniformNameType::eLightAtten: {
                        for (auto i=0; i<lights.size(); ++i) {
                            auto atten = lights[i]->getAttenuate();
                            lData[i * 4] = atten.x;
                            lData[i * 4 + 1] = atten.y;
                            lData[i * 4 + 2] = atten.z;
                        }
                        memcpy(data + uniform.offset/sizeof(float), lData, varArraySize);
                    }
                        break;
                    case UniformNameType::eLightAmbient: {
                        for (auto i=0; i<lights.size(); ++i) {
                            auto ambient = lights[i]->getAmbient();
                            lData[i * 4] = ambient.r;
                            lData[i * 4 + 1] = ambient.g;
                            lData[i * 4 + 2] = ambient.b;
                        }
                        memcpy(data + uniform.offset/sizeof(float), lData, varArraySize);
                    }
                        break;
                    case UniformNameType::eLightColor: {
                        for (auto i=0; i<lights.size(); ++i) {
                            auto color = lights[i]->getColor();
                            lData[i * 4] = color.r;
                            lData[i * 4 + 1] = color.g;
                            lData[i * 4 + 2] = color.b;
                        }
                        memcpy(data + uniform.offset/sizeof(float), lData, varArraySize);
                    }
                        break;
                    case UniformNameType::eEyeDir: {
                        auto curPos = SceneManager::getSingleton().getActiveCamera()->getForward();
                        lData[0] = curPos.x; lData[1] = curPos.y; lData[2] = curPos.z;
                        memcpy(data + uniform.offset/sizeof(float), lData, sizeof(float) * 3);
                    }
                        break;
                    default:
                        break;
                }
            }
            glUnmapBuffer(GL_UNIFORM_BUFFER);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }
    }
    
    void ProgramGL::updateObjectShadowsUniforms(const std::vector<LightPtr>& shadows)
    {
        Peach3DAssert(shadows.size() == mShadowCount, "Shadow list must equal to program count!");
        if (mShadowUBOId != GL_INVALID_INDEX && mShadowCount > 0) {
            glBindBuffer(GL_UNIFORM_BUFFER, mShadowUBOId);
            // map shadow buffer and copy memory on GL3
            float* data = (float*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, mShadowUBOSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
            for (auto& uniform : mShadowUBOUniforms) {
                switch (ShaderCode::getUniformNameType(uniform.name)) {
                    case UniformNameType::eShadowMatrix: {
                        for (auto i=0; i<shadows.size(); ++i) {
                            memcpy(data + i * 16, shadows[i]->getShadowMatrix().mat, 16 * sizeof(float));
                        }
                    }
                        break;
                    default:
                        break;
                }
            }
            glUnmapBuffer(GL_UNIFORM_BUFFER);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }
    }
    
    void ProgramGL::updateObjectBoneUniforms(const SkeletonPtr& sk, const std::vector<std::string>& names)
    {
        Peach3DAssert(names.size() == mBoneCount, "Bone count must equal to program count!");
        if (mBoneUBOId != GL_INVALID_INDEX && mBoneUBOSize > 0) {
            glBindBuffer(GL_UNIFORM_BUFFER, mBoneUBOId);
            // map shadow buffer and copy memory on GL3
            float* data = (float*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, mBoneUBOSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
            for (auto& uniform : mBoneUBOUniforms) {
                switch (ShaderCode::getUniformNameType(uniform.name)) {
                    case UniformNameType::eBoneMatrix: {
                        auto matrixList = sk->getBonesAnimMatrix(names);
                        for (auto i=0; i<mBoneCount; ++i) {
                            float* originData = matrixList[i].mat;
                            data[i * 12 + 0] = originData[0];
                            data[i * 12 + 1] = originData[4];
                            data[i * 12 + 2] = originData[8];
                            data[i * 12 + 3] = originData[12];
                            
                            data[i * 12 + 4] = originData[1];
                            data[i * 12 + 5] = originData[5];
                            data[i * 12 + 6] = originData[9];
                            data[i * 12 + 7] = originData[13];
                            
                            data[i * 12 + 8] = originData[2];
                            data[i * 12 + 9] = originData[6];
                            data[i * 12 + 10] = originData[10];
                            data[i * 12 + 11] = originData[14];
                        }
                    }
                        break;
                    default:
                        break;
                }
            }
            glUnmapBuffer(GL_UNIFORM_BUFFER);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }
    }
    
    void ProgramGL::updateGlobalObjectUniforms()
    {
        if (mObjectUBOId != GL_INVALID_INDEX) {
            glBindBuffer(GL_UNIFORM_BUFFER, mObjectUBOId);
            // map global buffer and copy memory on GL3
            float* data = (float*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, mObjectUBOSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
            SceneManager* sgr = SceneManager::getSingletonPtr();
            for (auto& uniform : mObjectUBOUniforms) {
                switch (ShaderCode::getUniformNameType(uniform.name)) {
                    case UniformNameType::eProjMatrix: {
                        const Matrix4& projMatrix = sgr->getProjectionMatrix();
                        memcpy(data + uniform.offset/sizeof(float), projMatrix.mat, sizeof(float)*16);
                    }
                        break;
                    case UniformNameType::eViewMatrix: {
                        const Matrix4& viewMatrix = sgr->getActiveCamera()->getViewMatrix();
                        memcpy(data + uniform.offset/sizeof(float), viewMatrix.mat, sizeof(float)*16);
                    }
                        break;
                    default:
                        break;
                }
            }
            glUnmapBuffer(GL_UNIFORM_BUFFER);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }
    }
    
    void ProgramGL::updateGlobalWidgetUniforms()
    {
        if (mWidgetUBOId != GL_INVALID_INDEX) {
            glBindBuffer(GL_UNIFORM_BUFFER, mWidgetUBOId);
            // map global buffer and copy memory on GL3
            float* data = (float*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, mWidgetUBOSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
            for (auto& uniform : mWidgetUBOUniforms) {
                switch (ShaderCode::getUniformNameType(uniform.name)) {
                    case UniformNameType::eViewRect: {
                        const Vector2& winSize = LayoutManager::getSingleton().getScreenSize();
                        float viewRect[] = {0.0f, 0.0f, winSize.x, winSize.y};
                        memcpy(data + uniform.offset/sizeof(float), viewRect, sizeof(float) * 4);
                    }
                        break;
                    default:
                        break;
                }
            }
            glUnmapBuffer(GL_UNIFORM_BUFFER);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }
    }
    
    void ProgramGL::bindProgramVertexAttrib()
    {
        if (mAttriBuffer && mDrawInstance) {
            glBindBuffer(GL_ARRAY_BUFFER, mAttriBuffer);
            
            // bind program instace buffer
            GLuint curOffset = 0;
            for (auto& uniform : mProgramUniformList) {
                uint uniformFloatSize = ShaderCode::getUniformFloatBits(uniform.dType);
                uint repeatCount = (uniform.dType == UniformDataType::eMatrix4) ? 4 : 1;
                uniformFloatSize = (uniform.dType == UniformDataType::eMatrix4) ? (uniformFloatSize / 4) : uniformFloatSize;
                
                // get current uniform locate by name
                GLint curLocate = glGetAttribLocation(mProgram, uniform.name.c_str());
                Peach3DAssert(curLocate >= 0, "Can't get uniform locate from name!");
                if (curLocate >= 0) {
                    for (auto i = 0; i < repeatCount; ++i) {
                        glEnableVertexAttribArray(curLocate);
                        glVertexAttribPointer(curLocate, uniformFloatSize, GL_FLOAT, GL_FALSE, mUniformsSize,
                                              PEACH3D_BUFFER_OFFSET(curOffset));
                        glVertexAttribDivisor(curLocate, 1);
                        curOffset += uniformFloatSize * sizeof(float);
                        curLocate ++;
                    }
                }
            }
        }
    }
    
    float* ProgramGL::beginMapInstanceUniformBuffer(uint count)
    {
        // resize render buffer size
        uint needCount = count - mInstancedCount;
        if (needCount > 0 && mDrawInstance) {
            // save current instanced data count
            uint formulaCount = std::min(mInstancedCount, (uint)INSTANCED_COUNT_INCREASE_STEP);
            mInstancedCount = mInstancedCount + std::max(needCount, formulaCount);
            
            // generate attribute buffer, vertex array have bind now
            if (mAttriBuffer == 0) {
                glGenBuffers(1, &mAttriBuffer);
            }
            glBindBuffer(GL_ARRAY_BUFFER, mAttriBuffer);
            // bind instanced uniform buffer
            glBufferData(GL_ARRAY_BUFFER, mUniformsSize * mInstancedCount, nullptr, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        
        float* data = nullptr;
        if (mAttriBuffer) {
            // copy attri data to instanced buffer
            glBindBuffer(GL_ARRAY_BUFFER, mAttriBuffer);
            uint copySize = mUniformsSize * count;
            data = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, copySize,
                                            GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
        }
        return data;
    }

    void ProgramGL::endMapInstanceUniformBuffer()
    {
        // unmap instanced attribute buffer
        glUnmapBuffer(GL_ARRAY_BUFFER);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
    void ProgramGL::activeTextures(GLuint texId, uint index, const std::string& uName)
    {
        if (texId > 0) {
            // bind one texture
            glActiveTexture(GL_TEXTURE0 + index);
            glBindTexture(GL_TEXTURE_2D, texId);
            auto usedName = uName;
            if (usedName.empty()) {
                usedName = Utils::formatString("pd_texture%d", index);
            }
            setUniformLocationValue(usedName.c_str(), [&index](GLint location) {
                glUniform1i(location, index);
            });
        }
    }
    
    void ProgramGL::setUniformLocationValue(const std::string& name, std::function<void(GLint)> valueFunc)
    {
        GLint uniformLocation = -1;
        if (mUniformLocateMap.find(name) == mUniformLocateMap.end()) {
            uniformLocation = glGetUniformLocation(mProgram, name.c_str());
            if (uniformLocation < 0) {
                Peach3DLog(LogLevel::eError, "Program %u can't find the uniform name %s", mProgramId, name.c_str());
            }
            mUniformLocateMap[name] = uniformLocation;
        }
        else {
            uniformLocation = mUniformLocateMap[name];
        }
        // set value if location exist
        if (uniformLocation >= 0) {
            valueFunc(uniformLocation);
        }
    }
    
    void ProgramGL::updateLightingUniformsGL2(const std::vector<LightPtr>& lights, const std::vector<LightPtr>& shadows, const Matrix4& normalMat, const Material& objMat)
    {
        // lights attribute
        float lData[16 * SceneManager::getSingleton().getLightMax()];
        // update object uniforms in list
        for (auto& uniform : mProgramUniformList) {
            // lights uniforms
            switch (ShaderCode::getUniformNameType(uniform.name)) {
                case UniformNameType::eLightTypeSpot:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        for (auto i=0; i<lights.size(); ++i) {
                            auto spotExt = lights[i]->getSpotExtend();
                            lData[i * 4] = (float)lights[i]->getType();
                            lData[i * 4 + 1] = spotExt.x;
                            lData[i * 4 + 2] = spotExt.y;
                            lData[i * 4 + 3] = lights[i]->getShadowTexture() ? 1.f: 0.f;
                        }
                        glUniform4fv(location, (GLsizei)lights.size(), lData);
                    });
                    break;
                case UniformNameType::eLightPos:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        for (auto i=0; i<lights.size(); ++i) {
                            auto pos = lights[i]->getPosition();
                            lData[i * 3] = pos.x;
                            lData[i * 3 + 1] = pos.y;
                            lData[i * 3 + 2] = pos.z;
                        }
                        glUniform3fv(location, (GLsizei)lights.size(), lData);
                    });
                    break;
                case UniformNameType::eLightDir:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        for (auto i=0; i<lights.size(); ++i) {
                            auto dir = lights[i]->getDirection();
                            lData[i * 3] = dir.x;
                            lData[i * 3 + 1] = dir.y;
                            lData[i * 3 + 2] = dir.z;
                        }
                        glUniform3fv(location, (GLsizei)lights.size(), lData);
                    });
                    break;
                case UniformNameType::eLightAtten:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        for (auto i=0; i<lights.size(); ++i) {
                            auto attenuate = lights[i]->getAttenuate();
                            lData[i * 3] = attenuate.x;
                            lData[i * 3 + 1] = attenuate.y;
                            lData[i * 3 + 2] = attenuate.z;
                        }
                        glUniform3fv(location, (GLsizei)lights.size(), lData);
                    });
                    break;
                case UniformNameType::eLightAmbient:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        for (auto i=0; i<lights.size(); ++i) {
                            auto ambient = lights[i]->getAmbient();
                            lData[i * 3] = ambient.r;
                            lData[i * 3 + 1] = ambient.g;
                            lData[i * 3 + 2] = ambient.b;
                        }
                        glUniform3fv(location, (GLsizei)lights.size(), lData);
                    });
                    break;
                case UniformNameType::eLightColor:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        for (auto i=0; i<lights.size(); ++i) {
                            auto color = lights[i]->getColor();
                            lData[i * 3] = color.r;
                            lData[i * 3 + 1] = color.g;
                            lData[i * 3 + 2] = color.b;
                        }
                        glUniform3fv(location, (GLsizei)lights.size(), lData);
                    });
                    break;
                case UniformNameType::eEyeDir:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        auto curPos = SceneManager::getSingleton().getActiveCamera()->getForward();
                        lData[0] = curPos.x; lData[1] = curPos.y; lData[2] = curPos.z;
                        glUniform3fv(location, 1, lData);
                    });
                    break;
                    // shadow uniforms
                case UniformNameType::eShadowMatrix:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        for (auto i=0; i<shadows.size(); ++i) {
                            auto sm = shadows[i]->getShadowMatrix();
                            memcpy(lData + i * 16, sm.mat, sizeof(float) * 16);
                        }
                        glUniformMatrix4fv(location, (GLsizei)shadows.size(), false, lData);
                    });
                    break;
                    // material uniforms
                case UniformNameType::eNormalMatrix:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        Matrix4 invmat, model = normalMat;
                        model.getInverse(&invmat);
                        glUniformMatrix4fv(location, 1, false, invmat.getTranspose().mat);
                    });
                    break;
                case UniformNameType::eMatAmbient:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        float color[] = {objMat.ambient.r, objMat.ambient.g, objMat.ambient.b};
                        glUniform3fv(location, 1, color);
                    });
                    break;
                case UniformNameType::eMatSpecular:
                    // also pass shininess
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        float color[] = {objMat.specular.r, objMat.specular.g, objMat.specular.b, objMat.shininess};
                        glUniform4fv(location, 1, color);
                    });
                    break;
                case UniformNameType::eMatEmissive:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        float color[] = {objMat.emissive.r, objMat.emissive.g, objMat.emissive.b};
                        glUniform3fv(location, 1, color);
                    });
                    break;
                default:
                    break;
            }
        }
    }
    
    void ProgramGL::updateWorldUniformsGL2(const Matrix4& modelMat, const Color4& diffuse)
    {
        SceneManager* sgr = SceneManager::getSingletonPtr();
        for (auto& uniform : mProgramUniformList) {
            switch (ShaderCode::getUniformNameType(uniform.name)) {
                case UniformNameType::eProjMatrix:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        const Matrix4& projMatrix = sgr->getProjectionMatrix();
                        glUniformMatrix4fv(location, 1, false, projMatrix.mat);
                    });
                    break;
                case UniformNameType::eViewMatrix:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        const Matrix4& viewMatrix = sgr->getActiveCamera()->getViewMatrix();
                        glUniformMatrix4fv(location, 1, false, viewMatrix.mat);
                    });
                    break;
                case UniformNameType::eModelMatrix:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        glUniformMatrix4fv(location, 1, false, modelMat.mat);
                    });
                    break;
                case UniformNameType::eDiffuse:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        float color[] = {diffuse.r, diffuse.g, diffuse.b, diffuse.a};
                        glUniform4fv(location, 1, color);
                    });
                    break;
                default:
                    break;
            }
        }
    }

    void ProgramGL::updateRenderNodeUniforms(RenderNode* node)
    {
        const Material& objMat = node->getMaterial();
        auto& modelMat = node->getModelMatrix();
        updateWorldUniformsGL2(modelMat, Color4(objMat.diffuse, objMat.alpha));
        // update object uniforms in list
        for (auto& uniform : mProgramUniformList) {
            switch (ShaderCode::getUniformNameType(uniform.name)) {
                    // bone uniforms
                case UniformNameType::eBoneMatrix:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        auto& sk = node->getBindSkeleton();
                        auto& boneNames = node->getObject()->getUsedBones();
                        auto matrixList = sk->getBonesAnimMatrix(boneNames);
                        float mData[12 * matrixList.size()];
                        for (auto i=0; i<matrixList.size(); ++i) {
                            float* originData = matrixList[i].mat;
                            mData[i * 12 + 0] = originData[0];
                            mData[i * 12 + 1] = originData[4];
                            mData[i * 12 + 2] = originData[8];
                            mData[i * 12 + 3] = originData[12];
                            
                            mData[i * 12 + 4] = originData[1];
                            mData[i * 12 + 5] = originData[5];
                            mData[i * 12 + 6] = originData[9];
                            mData[i * 12 + 7] = originData[13];
                            
                            mData[i * 12 + 8] = originData[2];
                            mData[i * 12 + 9] = originData[6];
                            mData[i * 12 + 10] = originData[10];
                            mData[i * 12 + 11] = originData[14];
                        }
                        glUniform4fv(location, (GLsizei)matrixList.size() * 3, mData);
                    });
                    break;
                default:
                    break;
            }
        }
        // not using lights and shadow when rendering shadow texture
        std::vector<LightPtr> lights, shadows;
        if (!node->isRenderForShadow()) {
            lights = node->getRenderLights();
            shadows = node->getShadowLights();
            updateLightingUniformsGL2(lights, shadows, modelMat, objMat);
        }
    }
    
    void ProgramGL::updateWorldUniformsGL3(float *startBuffer, const Matrix4& modelMat, const Material& objMat)
    {
        int startOffset = 0;
        for (auto& uniform : mProgramUniformList) {
            switch (ShaderCode::getUniformNameType(uniform.name)) {
                case UniformNameType::eModelMatrix: {
                    memcpy(startBuffer + startOffset, modelMat.mat, 16 * sizeof(float));
                    startOffset += 16;
                }
                    break;
                case UniformNameType::eDiffuse: {
                    float color[] = {objMat.diffuse.r, objMat.diffuse.g, objMat.diffuse.b, objMat.alpha};
                    memcpy(startBuffer + startOffset, color, 4 * sizeof(float));
                    startOffset += 4;
                }
                    break;
                    // material uniforms
                case UniformNameType::eNormalMatrix: {
                    Matrix4 calcMat = modelMat;
                    Matrix4 invmat;
                    calcMat.getInverse(&invmat);
                    memcpy(startBuffer + startOffset, invmat.getTranspose().mat, 16 * sizeof(float));
                    startOffset += 16;
                }
                    break;
                case UniformNameType::eMatAmbient: {
                    float color[] = {objMat.ambient.r, objMat.ambient.g, objMat.ambient.b};
                    memcpy(startBuffer + startOffset, color, 3 * sizeof(float));
                    startOffset += 3;
                }
                    break;
                case UniformNameType::eMatSpecular: {
                    // also pass shininess
                    float color[] = {objMat.specular.r, objMat.specular.g, objMat.specular.b, objMat.shininess};
                    memcpy(startBuffer + startOffset, color, 4 * sizeof(float));
                    startOffset += 4;
                }
                    break;
                case UniformNameType::eMatEmissive: {
                    float color[] = {objMat.emissive.r, objMat.emissive.g, objMat.emissive.b};
                    memcpy(startBuffer + startOffset, color, 3 * sizeof(float));
                    startOffset += 3;
                }
                    break;
                default:
                    break;
            }
        }
    }
    
    void ProgramGL::updateInstancedRenderNodeUniforms(const std::vector<RenderNode*>& renderList)
    {
        // map insanced attribute buffer
        float *data = beginMapInstanceUniformBuffer((uint)renderList.size());
        
        if (data) {
            // set object params to data
            for (auto i = 0; i < renderList.size(); ++i) {
                int uniformOffset = (mUniformsSize / 4) * i;
                updateWorldUniformsGL3(data + uniformOffset, renderList[i]->getModelMatrix(), renderList[i]->getMaterial());
            }
            
            // unmap instanced attribute buffer
            endMapInstanceUniformBuffer();
        }
    }
    
    void ProgramGL::updateOBBUniforms(OBB* obb)
    {
        SceneManager* sgr = SceneManager::getSingletonPtr();
        Color4 OBBColor = IRender::getSingleton().getRenderOBBColor();
        // update object uniforms in list
        for (auto& uniform : mProgramUniformList) {
            switch (ShaderCode::getUniformNameType(uniform.name)) {
                case UniformNameType::eProjMatrix:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        const Matrix4& projMatrix = sgr->getProjectionMatrix();
                        glUniformMatrix4fv(location, 1, false, projMatrix.mat);
                    });
                    break;
                case UniformNameType::eViewMatrix:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        const Matrix4& viewMatrix = sgr->getActiveCamera()->getViewMatrix();
                        glUniformMatrix4fv(location, 1, false, viewMatrix.mat);
                    });
                    break;
                case UniformNameType::eModelMatrix:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        const Matrix4& modelMat = obb->getModelMatrix();
                        glUniformMatrix4fv(location, 1, false, modelMat.mat);
                    });
                    break;
                case UniformNameType::eDiffuse:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        float color[] = {OBBColor.r, OBBColor.g, OBBColor.b, OBBColor.a};
                        glUniform4fv(location, 1, color);
                    });
                    break;
                default:
                    break;
            }
        }
    }
    
    void ProgramGL::updateInstancedOBBUniforms(const std::vector<OBB*>& renderList)
    {
        // map insanced attribute buffer
        float *data = beginMapInstanceUniformBuffer((uint)renderList.size());
        
        if (data) {
            Color4 OBBColor = IRender::getSingleton().getRenderOBBColor();
            // set object params to data
            for (auto i = 0; i < renderList.size(); ++i) {
                int startOffset = 0;
                int uniformOffset = (mUniformsSize / 4) * i;
                for (auto& uniform : mProgramUniformList) {
                    switch (ShaderCode::getUniformNameType(uniform.name)) {
                        case UniformNameType::eModelMatrix: {
                            const Matrix4& modelMat = renderList[i]->getModelMatrix();
                            memcpy(data + uniformOffset + startOffset, modelMat.mat, 16 * sizeof(float));
                            startOffset += 16;
                        }
                            break;
                        case UniformNameType::eDiffuse: {
                            float color[] = {OBBColor.r, OBBColor.g, OBBColor.b, OBBColor.a};
                            memcpy(data + uniformOffset + startOffset, color, 4 * sizeof(float));
                            startOffset += 4;
                        }
                            break;
                        default:
                            break;
                    }
                }
            }
            
            // unmap instanced attribute buffer
            endMapInstanceUniformBuffer();
        }
    }
    
    void ProgramGL::updateWidgetUniforms(Widget* widget)
    {
        const Vector2& winSize = LayoutManager::getSingleton().getScreenSize();
        // update widget uniforms in list
        for (auto& uniform : mProgramUniformList) {
            switch (ShaderCode::getUniformNameType(uniform.name)) {
                case UniformNameType::eViewRect:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        float viewRect[] = {0.0f, 0.0f, winSize.x, winSize.y};
                        glUniform4fv(location, 1, viewRect);
                    });
                    break;
                case UniformNameType::eShowRect:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        const Vector2& pos = widget->getPosition(TranslateRelative::eWorld);
                        const Vector2& size = widget->getContentSize(TranslateRelative::eWorld);
                        float rect[] = {pos.x, pos.y, size.x, size.y};
                        glUniform4fv(location, 1, rect);
                    });
                    break;
                case UniformNameType::eAnRot:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        const Vector2& anchor = widget->getAnchorPoint();
                        float anRot[] = {anchor.x, anchor.y, widget->getRotation(TranslateRelative::eWorld)};
                        glUniform3fv(location, 1, anRot);
                    });
                    break;
                case UniformNameType::ePatShowRect:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        Vector2 pos;
                        Vector2 size = winSize;
                        Widget* patWidget = static_cast<Widget*>(widget->getParentNode());
                        if (widget->isClipEnabled() && patWidget) {
                            pos = patWidget->getPosition(TranslateRelative::eWorld);
                            size = patWidget->getContentSize(TranslateRelative::eWorld);
                        }
                        float rect[] = {pos.x, pos.y, size.x, size.y};
                        glUniform4fv(location, 1, rect);
                    });
                    break;
                case UniformNameType::ePatAnRot:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        Vector2 anchor;
                        float rotate = 0.0f;
                        Widget* patWidget = static_cast<Widget*>(widget->getParentNode());
                        if (widget->isClipEnabled() && patWidget) {
                            rotate = patWidget->getRotation(TranslateRelative::eWorld);
                            anchor = patWidget->getAnchorPoint();
                        }
                        float anRot[] = {anchor.x, anchor.y, rotate};
                        glUniform3fv(location, 1, anRot);
                    });
                    break;
                case UniformNameType::eDiffuse:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        auto color = widget->getColor();
                        float colour[] = {color.r, color.g, color.b, widget->getRenderAlpha()};
                        glUniform4fv(location, 1, colour);
                    });
                    break;
                case UniformNameType::eUVRect:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        const Rect& texCoord = static_cast<Sprite*>(widget)->getTextureFrame().rc;
                        float coord[] = {texCoord.pos.x, texCoord.pos.y, texCoord.size.x, texCoord.size.y};
                        glUniform4fv(location, 1, coord);
                    });
                    break;
                case UniformNameType::eTexEffect:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        const Vector2& scale = widget->getScale();
                        float gray = static_cast<Sprite*>(widget)->isGrayscaleEnabled() ? 1.f : 0.f;
                        float effect[] = {scale.x > 0.f ? 1.f : -1.f, scale.y > 0.f ? 1.f : -1.f, gray};
                        glUniform3fv(location, 1, effect);
                    });
                    break;
                default:
                    break;
            }
        }
    }
    
    void ProgramGL::updateInstancedWidgetUniforms(const std::vector<Widget*>& renderList)
    {
        // map insanced attribute buffer
        float *data = beginMapInstanceUniformBuffer((uint)renderList.size());
        
        if (data) {
            const Vector2& winSize = LayoutManager::getSingleton().getScreenSize();
            // set widget params to data
            for (auto i = 0; i < renderList.size(); ++i) {
                int startOffset = 0;
                int uniformOffset = (mUniformsSize / 4) * i;
                for (auto& uniform : mProgramUniformList) {
                    switch (ShaderCode::getUniformNameType(uniform.name)) {
                        case UniformNameType::eShowRect: {
                            const Vector2& pos = renderList[i]->getPosition(TranslateRelative::eWorld);
                            const Vector2& size = renderList[i]->getContentSize(TranslateRelative::eWorld);
                            float rect[] = {pos.x, pos.y, size.x, size.y};
                            memcpy(data + uniformOffset + startOffset, rect, 4 * sizeof(float));
                            startOffset += 4;
                        }
                            break;
                        case UniformNameType::eAnRot: {
                            const Vector2& anchor = renderList[i]->getAnchorPoint();
                            float anRot[] = {anchor.x, anchor.y, renderList[i]->getRotation(TranslateRelative::eWorld)};
                            memcpy(data + uniformOffset + startOffset, anRot, 3 * sizeof(float));
                            startOffset += 3;
                        }
                            break;
                        case UniformNameType::ePatShowRect: {
                            Vector2 pos;
                            Vector2 size = winSize;
                            Widget* patWidget = static_cast<Widget*>(renderList[i]->getParentNode());
                            if (renderList[i]->isClipEnabled() && patWidget) {
                                pos = patWidget->getPosition(TranslateRelative::eWorld);
                                size = patWidget->getContentSize(TranslateRelative::eWorld);
                            }
                            float rect[] = {pos.x, pos.y, size.x, size.y};
                            memcpy(data + uniformOffset + startOffset, rect, 4 * sizeof(float));
                            startOffset += 4;
                        }
                            break;
                        case UniformNameType::ePatAnRot: {
                            Vector2 anchor;
                            float rotate = 0.0f;
                            Widget* patWidget = static_cast<Widget*>(renderList[i]->getParentNode());
                            if (renderList[i]->isClipEnabled() && patWidget) {
                                rotate = patWidget->getRotation(TranslateRelative::eWorld);
                                anchor = patWidget->getAnchorPoint();
                            }
                            float anRot[] = {anchor.x, anchor.y, rotate};
                            memcpy(data + uniformOffset + startOffset, anRot, 3 * sizeof(float));
                            startOffset += 3;
                        }
                            break;
                        case UniformNameType::eDiffuse: {
                            auto color = renderList[i]->getColor();
                            float colour[] = {color.r, color.g, color.b, renderList[i]->getRenderAlpha()};
                            memcpy(data + uniformOffset + startOffset, colour, 4 * sizeof(float));
                            startOffset += 4;
                        }
                            break;
                        case UniformNameType::eUVRect: {
                            const Rect& texCoord = static_cast<Sprite*>(renderList[i])->getTextureFrame().rc;
                            float coord[] = {texCoord.pos.x, texCoord.pos.y, texCoord.size.x, texCoord.size.y};
                            memcpy(data + uniformOffset + startOffset, coord, 4 * sizeof(float));
                            startOffset += 4;
                        }
                            break;
                        case UniformNameType::eTexEffect: {
                            const Vector2& scale = renderList[i]->getScale();
                            float gray = static_cast<Sprite*>(renderList[i])->isGrayscaleEnabled() ? 1.f : 0.f;
                            float effect[] = {scale.x > 0.f ? 1.f : -1.f, scale.y > 0.f ? 1.f : -1.f, gray};
                            memcpy(data + uniformOffset + startOffset, effect, 3 * sizeof(float));
                            startOffset += 3;
                        }
                            break;
                        default:
                            break;
                    }
                }
            }
            
            // unmap instanced attribute buffer
            endMapInstanceUniformBuffer();
        }
    }
    
    void ProgramGL::updateParticle2DUniforms(const Rect& coord)
    {
        const Vector2& winSize = LayoutManager::getSingleton().getScreenSize();
        // update widget uniforms in list
        for (auto& uniform : mProgramUniformList) {
            switch (ShaderCode::getUniformNameType(uniform.name)) {
                case UniformNameType::eViewRect:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        float viewRect[] = {0.0f, 0.0f, winSize.x, winSize.y};
                        glUniform4fv(location, 1, viewRect);
                    });
                    break;
                case UniformNameType::eUVRect:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        float viewRect[] = {coord.pos.x, coord.pos.y, coord.size.x, coord.size.y};
                        glUniform4fv(location, 1, viewRect);
                    });
                    break;
                default:
                    break;
            }
        }
    }
    
    void ProgramGL::updateParticle3DUniforms(const Rect& coord)
    {
        SceneManager* sgr = SceneManager::getSingletonPtr();
        // update widget uniforms in list
        for (auto& uniform : mProgramUniformList) {
            switch (ShaderCode::getUniformNameType(uniform.name)) {
                case UniformNameType::eProjMatrix:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        const Matrix4& projMatrix = sgr->getProjectionMatrix();
                        glUniformMatrix4fv(location, 1, false, projMatrix.mat);
                    });
                    break;
                case UniformNameType::eViewMatrix:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        const Matrix4& viewMatrix = sgr->getActiveCamera()->getViewMatrix();
                        glUniformMatrix4fv(location, 1, false, viewMatrix.mat);
                    });
                    break;
                case UniformNameType::eUVRect:
                    setUniformLocationValue(uniform.name, [&](GLint location) {
                        float viewRect[] = {coord.pos.x, coord.pos.y, coord.size.x, coord.size.y};
                        glUniform4fv(location, 1, viewRect);
                    });
                    break;
                default:
                    break;
            }
        }
    }
    
    void ProgramGL::updateTerrainUniforms(Terrain* ter)
    {
        auto modelMat = Matrix4::createTranslation(ter->getPosition());
        updateWorldUniformsGL2(modelMat, Color4White);
        // set lighting unfo, terrain will not rendering here
        std::vector<LightPtr> lights = ter->getRenderLights(), shadows = ter->getShadowLights();
        updateLightingUniformsGL2(lights, shadows, modelMat, ter->getMaterial());
        // add texture terrain
        for (auto& uniform : mProgramUniformList) {
            if (ShaderCode::getUniformNameType(uniform.name) ==  UniformNameType::eDetailSize) {
                auto details = ter->getBrushDetails();
                setUniformLocationValue(uniform.name, [&](GLint location) {
                    glUniform1fv(location, (GLsizei)details.size(), &details[0]);
                });
            }
        }
    }
        
    bool ProgramGL::useAsRenderProgram()
    {
        if (mProgram && mProgramValid) {
            glUseProgram(mProgram);
            
            // bind uniform buffer for GL3, particle not need
            //! this is very Important, just like bind array buffer and index buffer
            if (mDrawInstance) {
                glBindBufferBase(GL_UNIFORM_BUFFER, GLOBAL_UBO_BINDING_POINT,
                                 (mVertexType & VertexType::Point3) ? mObjectUBOId: mWidgetUBOId);
                if (mLightsCount > 0) {
                    glBindBufferBase(GL_UNIFORM_BUFFER, LIGHTS_UBO_BINDING_POINT, mLightsUBOId);
                    if (mShadowCount > 0) {
                        glBindBufferBase(GL_UNIFORM_BUFFER, SHADOW_UBO_BINDING_POINT, mShadowUBOId);
                    }
                }
                if (mBoneCount > 0) {
                    glBindBufferBase(GL_UNIFORM_BUFFER, BONES_UBO_BINDING_POINT, mBoneUBOId);
                }
            }
            return true;
        }
        return false;
    }

    void ProgramGL::deleteGlobalUBO()
    {
        // delete global Ubo when render exit
        if (mWidgetUBOId != GL_INVALID_INDEX) {
            glDeleteBuffers(1, &mWidgetUBOId);
            mWidgetUBOId = GL_INVALID_INDEX;
        }
        if (mObjectUBOId != GL_INVALID_INDEX) {
            glDeleteBuffers(1, &mObjectUBOId);
            mObjectUBOId = GL_INVALID_INDEX;
        }
    }
    
    ProgramGL::~ProgramGL()
    {
        if (mAttriBuffer) {
            glDeleteBuffers(1, &mAttriBuffer);
            mAttriBuffer = 0;
        }
        if (mProgram) {
            // delete program
            glDeleteProgram(mProgram);
            mProgram = 0;
        }
        if (mVSShader) {
            // delete vertex shader when it's exist
            glDeleteShader(mVSShader);
            mVSShader = 0;
        }
        if (mPSShader) {
            // delete pixel shader when it's exist
            glDeleteShader(mPSShader);
            mPSShader = 0;
        }
    }
}
