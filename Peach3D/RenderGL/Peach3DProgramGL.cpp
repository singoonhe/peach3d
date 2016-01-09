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
#include "Peach3DResourceManager.h"
#include "Peach3DSceneManager.h"

namespace Peach3D
{
    // patch widget count each auto increase
    #define INSTANCED_COUNT_INCREASE_STEP   50
    // define global UBO binding point
    #define GLOBAL_UBO_BINDING_POINT        0
    
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
        // compile shader, replace data if mVSShader exist
        mVSShader = loadShaderFromMemory(GL_VERTEX_SHADER, data, size);
        if (mVSShader && mPSShader && mVertexType) {
            compileProgram();
        }
        return (mVSShader > 0);
    }

    bool ProgramGL::setPixelShader(const char* data, int size, bool isCompiled)
    {
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
        RenderFeatureLevel featureLevel = IPlatform::getSingletonPtr()->getRenderFeatureLevel();
        if (featureLevel == RenderFeatureLevel::eGL3) {
            shaderStrings[0] = definedShader3;
            shaderStringLengths[0] = (GLint)strlen(definedShader3);
        }
        else if (featureLevel == RenderFeatureLevel::eGL2 && type == GL_VERTEX_SHADER) {
            shaderStrings[0] = definedVS2;
            shaderStringLengths[0] = (GLint)strlen(definedVS2);
        }
        else if (featureLevel == RenderFeatureLevel::eGL2 && type == GL_FRAGMENT_SHADER) {
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
            Peach3DLog(LogLevel::eError, "Compile type %s shader of program %u failed",
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
    
    void ProgramGL::setVertexType(uint type)
    {
        IProgram::setVertexType(type);
        // if vs and ps is created, compile program
        if (mVSShader && mPSShader) {
            compileProgram();
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
            
            // bind gobal uniforms when GL3 support
            if (PD_RENDERLEVEL() == RenderFeatureLevel::eGL3) {
                bindGlobalUniforms();
            }
        }
        
        // detach and delete shader after program compile
        glDetachShader(mProgram, mVSShader);
        glDetachShader(mProgram, mPSShader);
    }
    
    void ProgramGL::bindGlobalUniforms()
    {
        // get global uniform from program, so every program must include GlobalUniforms
        GLuint globalIndex = glGetUniformBlockIndex(mProgram, "GlobalUnifroms");
        Peach3DAssert(globalIndex != GL_INVALID_INDEX, "block GlobalUnifroms must include in vertex shader");
        
        if (globalIndex != GL_INVALID_INDEX) {
            GLuint* globalUBOId = (mVertexType & VertexTypePosition3) ? &mObjectUBOId : &mWidgetUBOId;
            GLint* globalUBOSize = (mVertexType & VertexTypePosition3) ? &mObjectUBOSize : &mWidgetUBOSize;
            auto uniformList = (mVertexType & VertexTypePosition3) ? &mObjectUBOUniforms : &mWidgetUBOUniforms;
            // create global UBO if it not exist
            if ((*globalUBOId) == GL_INVALID_INDEX) {
                // get global unifrom size, may different on platforms
                glGetActiveUniformBlockiv(mProgram, globalIndex, GL_UNIFORM_BLOCK_DATA_SIZE, globalUBOSize);
                
                glGenBuffers(1, globalUBOId);
                glBindBuffer(GL_UNIFORM_BUFFER, *globalUBOId);
                // for standard, buffer size = sizeof(projMatrix) + sizeof(viewMatrix) + sizeof(ambient)
                glBufferData(GL_UNIFORM_BUFFER, *globalUBOSize, NULL, GL_DYNAMIC_DRAW);
                
                // get offsets
                auto uniformCount = uniformList->size();
                const GLchar **names = (const GLchar **)malloc(sizeof(const GLchar *) * uniformCount);
                for (auto i=0; i<uniformCount; ++i) {
                    names[i] = (*uniformList)[i].name.c_str();
                }
                GLuint indices[uniformCount];
                glGetUniformIndices(mProgram, (GLsizei)uniformCount, names, indices);
                GLint offset[uniformCount];
                glGetActiveUniformsiv(mProgram, (GLsizei)uniformCount, indices, GL_UNIFORM_OFFSET, offset);
                for (auto i=0; i<uniformCount; ++i) {
                    (*uniformList)[i].offset = offset[i];
                }
                glBindBuffer(GL_UNIFORM_BUFFER, 0);
                free(names);
                
                if (mVertexType & VertexTypePosition3) {
                    Peach3DLog(LogLevel::eInfo, "Create global object UBO success for GL3");
                }
                else {
                    Peach3DLog(LogLevel::eInfo, "Create global widget UBO success for GL3");
                }
            }
            // bind global buffer to global uniform, provide global info
            glBindBufferBase(GL_UNIFORM_BUFFER, GLOBAL_UBO_BINDING_POINT, *globalUBOId);
            glUniformBlockBinding(mProgram, globalIndex, GLOBAL_UBO_BINDING_POINT);
        }
    }
    
    void ProgramGL::updateGlobalObjectUnifroms()
    {
        if (mObjectUBOId != GL_INVALID_INDEX) {
            glBindBuffer(GL_UNIFORM_BUFFER, mObjectUBOId);
            // map global buffer and copy memory on GL3
            float* data = (float*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, mObjectUBOSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
            SceneManager* sgr = SceneManager::getSingletonPtr();
            for (auto uniform : mObjectUBOUniforms) {
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
    
    void ProgramGL::updateGlobalWidgetUnifroms()
    {
        if (mWidgetUBOId != GL_INVALID_INDEX) {
            glBindBuffer(GL_UNIFORM_BUFFER, mWidgetUBOId);
            // map global buffer and copy memory on GL3
            float* data = (float*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, mWidgetUBOSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
            for (auto uniform : mWidgetUBOUniforms) {
                switch (ShaderCode::getUniformNameType(uniform.name)) {
                    case UniformNameType::eViewRect: {
                        const Vector2& winSize = IPlatform::getSingleton().getCreationParams().winSize;
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
    
    void ProgramGL::createInstancedAttriDataAndBuffer(uint count)
    {
        Peach3DAssert(count > 0, "Attribute data size must bigger than 0!");
        if (count > 0) {
            // save current instanced data count
            mInstancedCount = count;
            
            // generate attribute buffer, vertex array have bind now
            if (mAttriBuffer == 0) {
                glGenBuffers(1, &mAttriBuffer);
                glBindBuffer(GL_ARRAY_BUFFER, mAttriBuffer);
                // bind color attri
                GLuint curOffset = 0;
                for (auto uniform : mProgramUniformList) {
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
            else {
                glBindBuffer(GL_ARRAY_BUFFER, mAttriBuffer);
            }
            
            // bind instanced uniform buffer
            glBufferData(GL_ARRAY_BUFFER, mUniformsSize * count, nullptr, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }
    
    void ProgramGL::setProgramUniformsDesc(const std::vector<ProgramUniform>& uniformList)
    {
        IProgram::setProgramUniformsDesc(uniformList);
        
        if (PD_RENDERLEVEL() == RenderFeatureLevel::eGL3 && uniformList.size() > 0) {
            // save one instanced uniform buffer size
            mUniformsSize = 0;
            for (auto uniform : uniformList) {
                mUniformsSize += ShaderCode::getUniformFloatBits(uniform.dType) * sizeof(float);
            }
        }
    }
    
    void ProgramGL::activeTextures(GLuint texId, uint index)
    {
        if (texId > 0) {
            // bind one texture
            glActiveTexture(GL_TEXTURE0 + index);
            glBindTexture(GL_TEXTURE_2D, texId);
            std::string pdName = Utils::formatString("pd_texture%d", index);
            setUnifromLocationValue(pdName.c_str(), [&index](GLint location) {
                glUniform1i(location, index);
            });
        }
    }
    
    void ProgramGL::setUnifromLocationValue(const std::string& name, std::function<void(GLint)> valueFunc)
    {
        GLint uniformLocation = -1;
        if (mUniformLocateMap.find(name) == mUniformLocateMap.end()) {
            uniformLocation = glGetUniformLocation(mProgram, name.c_str());
            if (uniformLocation < 0) {
                Peach3DLog(LogLevel::eError, "Program %u can't find the attribute name %s", mProgramId, name.c_str());
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

    void ProgramGL::updateRenderNodeUnifroms(RenderNode* node)
    {
        SceneManager* sgr = SceneManager::getSingletonPtr();
        const Material& objMat = node->getMaterial();
        // update object uniforms in list
        for (auto uniform : mProgramUniformList) {
            switch (ShaderCode::getUniformNameType(uniform.name)) {
                case UniformNameType::eProjMatrix:
                    setUnifromLocationValue(uniform.name, [&](GLint location) {
                        const Matrix4& projMatrix = sgr->getProjectionMatrix();
                        glUniformMatrix4fv(location, 1, false, projMatrix.mat);
                    });
                    break;
                case UniformNameType::eViewMatrix:
                    setUnifromLocationValue(uniform.name, [&](GLint location) {
                        const Matrix4& viewMatrix = sgr->getActiveCamera()->getViewMatrix();
                        glUniformMatrix4fv(location, 1, false, viewMatrix.mat);
                    });
                    break;
                case UniformNameType::eModelMatrix:
                    setUnifromLocationValue(uniform.name, [&](GLint location) {
                        const Matrix4& modelMat = node->getModelMatrix();
                        glUniformMatrix4fv(location, 1, false, modelMat.mat);
                    });
                    break;
                case UniformNameType::eDiffuse:
                    setUnifromLocationValue(uniform.name, [&](GLint location) {
                        float color[] = {objMat.diffuse.r, objMat.diffuse.g, objMat.diffuse.b, objMat.diffuse.a};
                        glUniform4fv(location, 1, color);
                    });
                    break;
                default:
                    break;
            }
        }
    }
    
    void ProgramGL::updateInstancedRenderNodeUnifroms(const std::vector<RenderNode*>& renderList)
    {
        // resize render buffer size
        int needCount = (int)renderList.size() - mInstancedCount;
        if (needCount > 0) {
            int formulaCount = std::min(mInstancedCount, (uint)INSTANCED_COUNT_INCREASE_STEP);
            createInstancedAttriDataAndBuffer(mInstancedCount + std::max(needCount, formulaCount));
        }
        
        // copy attri data to instanced buffer
        glBindBuffer(GL_ARRAY_BUFFER, mAttriBuffer);
        uint copySize = mUniformsSize * (uint)renderList.size();
        float *data = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, copySize,
                                               GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
        
        // set object params to data
        for (auto i = 0; i < renderList.size(); ++i) {
            int startOffset = 0;
            int uniformOffset = (mUniformsSize / 4) * i;
            const Material& objMat = renderList[i]->getMaterial();
            for (auto uniform : mProgramUniformList) {
                switch (ShaderCode::getUniformNameType(uniform.name)) {
                    case UniformNameType::eModelMatrix: {
                        const Matrix4& modelMat = renderList[i]->getModelMatrix();
                        memcpy(data + uniformOffset + startOffset, modelMat.mat, 16 * sizeof(float));
                        startOffset += 16;
                    }
                        break;
                    case UniformNameType::eDiffuse: {
                        float color[] = {objMat.diffuse.r, objMat.diffuse.g, objMat.diffuse.b, objMat.diffuse.a};
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
        glUnmapBuffer(GL_ARRAY_BUFFER);
    }
    
    void ProgramGL::updateWidgetUnifroms(Widget* widget)
    {
        const Vector2& winSize = IPlatform::getSingleton().getCreationParams().winSize;
        // update widget uniforms in list
        for (auto uniform : mProgramUniformList) {
            switch (ShaderCode::getUniformNameType(uniform.name)) {
                case UniformNameType::eViewRect:
                    setUnifromLocationValue(uniform.name, [&](GLint location) {
                        float viewRect[] = {0.0f, 0.0f, winSize.x, winSize.y};
                        glUniform4fv(location, 1, viewRect);
                    });
                    break;
                case UniformNameType::eShowRect:
                    setUnifromLocationValue(uniform.name, [&](GLint location) {
                        const Vector2& pos = widget->getPosition(TranslateRelative::eWorld);
                        const Vector2& size = widget->getContentSize(TranslateRelative::eWorld);
                        float rect[] = {pos.x, pos.y, size.x, size.y};
                        glUniform4fv(location, 1, rect);
                    });
                    break;
                case UniformNameType::eAnRot:
                    setUnifromLocationValue(uniform.name, [&](GLint location) {
                        const Vector2& anchor = widget->getAnchorPoint();
                        float anRot[] = {anchor.x, anchor.y, widget->getRotation(TranslateRelative::eWorld)};
                        glUniform3fv(location, 1, anRot);
                    });
                    break;
                case UniformNameType::ePatShowRect:
                    setUnifromLocationValue(uniform.name, [&](GLint location) {
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
                    setUnifromLocationValue(uniform.name, [&](GLint location) {
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
                    setUnifromLocationValue(uniform.name, [&](GLint location) {
                        const Color4& color = widget->getColor();
                        float colour[] = {color.r, color.g, color.b, widget->getAlpha()};
                        glUniform4fv(location, 1, colour);
                    });
                    break;
                case UniformNameType::eUVRect:
                    setUnifromLocationValue(uniform.name, [&](GLint location) {
                        const Rect& texCoord = static_cast<Sprite*>(widget)->getTextureRect();
                        float coord[] = {texCoord.pos.x, texCoord.pos.y, texCoord.size.x, texCoord.size.y};
                        glUniform4fv(location, 1, coord);
                    });
                    break;
                case UniformNameType::eTexEffect:
                    setUnifromLocationValue(uniform.name, [&](GLint location) {
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
    
    void ProgramGL::updateInstancedWidgetUnifroms(const std::vector<Widget*>& renderList)
    {
        // resize render buffer size
        int needCount = (int)renderList.size() - mInstancedCount;
        if (needCount > 0) {
            int formulaCount = std::min(mInstancedCount, (uint)INSTANCED_COUNT_INCREASE_STEP);
            createInstancedAttriDataAndBuffer(mInstancedCount + std::max(needCount, formulaCount));
        }
        
        // copy attri data to instanced buffer
        glBindBuffer(GL_ARRAY_BUFFER, mAttriBuffer);
        uint copySize = mUniformsSize * (uint)renderList.size();
        float *data = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, copySize,
                                               GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
        
        const Vector2& winSize = IPlatform::getSingleton().getCreationParams().winSize;
        // set widget params to data
        for (auto i = 0; i < renderList.size(); ++i) {
            int startOffset = 0;
            int uniformOffset = (mUniformsSize / 4) * i;
            for (auto uniform : mProgramUniformList) {
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
                        const Color4& color = renderList[i]->getColor();
                        float colour[] = {color.r, color.g, color.b, renderList[i]->getAlpha()};
                        memcpy(data + uniformOffset + startOffset, colour, 4 * sizeof(float));
                        startOffset += 4;
                    }
                        break;
                    case UniformNameType::eUVRect: {
                        const Rect& texCoord = static_cast<Sprite*>(renderList[i])->getTextureRect();
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
        glUnmapBuffer(GL_ARRAY_BUFFER);
    }
    
    bool ProgramGL::useAsRenderProgram()
    {
        if (mProgram && mProgramValid) {
            glUseProgram(mProgram);
            
            // bind uniform buffer for GL3
            //! this is very Important, just like bind array buffer and index buffer
            if (PD_RENDERLEVEL() == RenderFeatureLevel::eGL3) {
                glBindBufferBase(GL_UNIFORM_BUFFER, GLOBAL_UBO_BINDING_POINT,
                                 (mVertexType & VertexTypePosition3) ? mObjectUBOId: mWidgetUBOId);
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
