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
#include "shader/pdPosColorUVShader3D.h"

namespace Peach3D
{
    const std::vector<ProgramUniform> gGlobalUniforms2D = {ProgramUniform("pd_viewRect", UniformDataType::eVector4),};
    const std::vector<ProgramUniform> gGlobalUniforms3D = {ProgramUniform("pd_projMatrix", UniformDataType::eMatrix4),
        ProgramUniform("pd_viewMatrix", UniformDataType::eMatrix4)};
    
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
            mShaderMap["PosColorVerShader2D"] = ShaderCode::generateShaderCodeData(gCommonVertexFunc2D, gPosColorVerShader2D);
            mShaderMap["PosColorFragShader2D"] = ShaderCode::generateShaderCodeData(gCommonFragClipFunc2D, gPosColorFragShader2D);
            mShaderMap["PosColorUVVerShader2D"] = ShaderCode::generateShaderCodeData(gCommonVertexFunc2D, gPosColorUVVerShader2D);
            mShaderMap["PosColorUVFragShader2D"] = ShaderCode::generateShaderCodeData(gCommonFragClipFunc2D, gPosColorUVFragShader2D);
            mShaderMap["PosColorVerShader3D"] = ShaderCode::generateShaderCodeData(gPosColorVerShader3D);
            mShaderMap["PosColorFragShader3D"] = ShaderCode::generateShaderCodeData(gPosColorFragShader3D);
            mShaderMap["PosColorUVVerShader3D"] = ShaderCode::generateShaderCodeData(gPosColorUVVerShader3D);
            mShaderMap["PosColorUVFragShader3D"] = ShaderCode::generateShaderCodeData(gPosColorUVFragShader3D);
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
                mergeUniformsToFront(gPosColorUVUniforms3D, gGlobalUniforms3D);
            }
            mUniformsMap["PosColorVerShader2D"] =  gPosColorUniforms2D;
            mUniformsMap["PosColorUVVerShader2D"] = gPosColorUVUniforms2D;
            mUniformsMap["PosColorVerShader3D"] = gPosColorUniforms3D;
            mUniformsMap["PosColorUVVerShader3D"] = gPosColorUVUniforms3D;
        }
        return mUniformsMap[name];
    }
}