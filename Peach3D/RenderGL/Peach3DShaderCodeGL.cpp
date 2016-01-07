//
//  Peach3DShaderCodeGL.cpp
//  Peach3DLib
//
//  Created by singoon.he on 7/1/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#include "Peach3DShaderCode.h"
#include "Peach3DIPlatform.h"
#include "shader/pdCommonFuncShader2D.h"
#include "shader/pdPosColorShader2D.h"
#include "shader/pdPosColorShader3D.h"
#include "shader/pdPosColorUVShader2D.h"

namespace Peach3D
{
    const std::vector<ProgramUniform> gGlobalUniforms2D = {ProgramUniform("pd_viewRect", UniformDataType::eVector4),};
    const std::vector<ProgramUniform> gGlobalUniforms3D = {ProgramUniform("pd_projMatrix", UniformDataType::eMatrix4),
        ProgramUniform("pd_viewMatrix", UniformDataType::eMatrix4)};
    
    ShaderCodeData generateShaderCodeData(const char* code1, const char* code2 = "")
    {
        ulong code1Len = strlen(code1), code2Len = strlen(code2);
        char* shaderData = (char*)malloc(code1Len + code2Len + 1);
        memcpy(shaderData, code1, code1Len);
        if (code2Len > 0) {
            memcpy(shaderData + code1Len, code2, code2Len);
        }
        return ShaderCodeData(shaderData, int(code1Len + code2Len));
    }
    
    void mergeUniformsToFront(std::vector<ProgramUniform>& dst, const std::vector<ProgramUniform>& source)
    {
        for (auto uniform : source) {
            dst.insert(dst.begin(), uniform);
        }
    }
    
    const ShaderCodeData& ShaderCode::getShaderCode(const std::string& name)
    {
        // add all shader code if list is empty
        if (mShaderMap.empty()) {
            mShaderMap["PosColorVerShader2D"] = generateShaderCodeData(gCommonVertexFunc2D, gPosColorVerShader2D);
            mShaderMap["PosColorFragShader2D"] = generateShaderCodeData(gCommonFragClipFunc2D, gPosColorFragShader2D);
            mShaderMap["PosColorUVVerShader2D"] = generateShaderCodeData(gCommonVertexFunc2D, gPosColorUVVerShader2D);
            mShaderMap["PosColorUVFragShader2D"] = generateShaderCodeData(gCommonFragClipFunc2D, gPosColorUVFragShader2D);
            mShaderMap["PosColorVerShader3D"] = generateShaderCodeData(gPosColorVerShader3D);
            mShaderMap["PosColorFragShader3D"] = generateShaderCodeData(gPosColorFragShader3D);
        }
        return mShaderMap[name];
    }
    
    const std::vector<ProgramUniform>& ShaderCode::getProgramUniforms(const std::string& name)
    {
        // add all shader uniforms if list is empty
        if (mUniformsMap.empty()) {
            if (PD_RENDERLEVEL() == RenderFeatureLevel::eGL2) {
                // gl2 need add global uniforms
                mergeUniformsToFront(gPosColorUniforms2D, gGlobalUniforms2D);
                mergeUniformsToFront(gPosColorUVUniforms2D, gGlobalUniforms2D);
                mergeUniformsToFront(gPosColorUniforms3D, gGlobalUniforms3D);
            }
            mUniformsMap["PosColorVerShader2D"] =  gPosColorUniforms2D;
            mUniformsMap["PosColorUVVerShader2D"] = gPosColorUVUniforms2D;
            mUniformsMap["PosColorVerShader3D"] = gPosColorUniforms3D;
        }
        return mUniformsMap[name];
    }
}