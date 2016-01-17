//
//  Peach3DShaderCodeGL.cpp
//  Peach3DLib
//
//  Created by singoon.he on 7/1/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#include "Peach3DShaderCode.h"
#include "Peach3DIPlatform.h"
#include "shader/pdPosColorShader2D.h"
#include "Peach3DResourceManager.h"

namespace Peach3D
{
    const ShaderCodeData& ShaderCode::getShaderCode(const std::string& name)
    {
        // add all shader code if list is empty
        if (mShaderMap.empty()) {
            mShaderMap["PosColorVerShader2D"] = ShaderCode::generateShaderCodeData(gPosColorShader2D);
            mShaderMap["PosColorFragShader2D"] = ShaderCode::generateShaderCodeData(gPosColorShader2D);
            //mShaderMap["PosColorUVVerShader2D"] = ShaderCode::generateShaderCodeData(gCommonVertexFunc2D, gPosColorUVVerShader2D);
            //mShaderMap["PosColorUVFragShader2D"] = ShaderCode::generateShaderCodeData(gCommonFragClipFunc2D, gPosColorUVFragShader2D);
            //mShaderMap["PosColorVerShader3D"] = ShaderCode::generateShaderCodeData(gPosColorVerShader3D);
            //mShaderMap["PosColorFragShader3D"] = ShaderCode::generateShaderCodeData(gPosColorFragShader3D);
            //mShaderMap["PosColorUVVerShader3D"] = ShaderCode::generateShaderCodeData(gPosColorUVVerShader3D);
            //mShaderMap["PosColorUVFragShader3D"] = ShaderCode::generateShaderCodeData(gPosColorUVFragShader3D);
        }
        return mShaderMap[name];
    }
    
    const std::vector<ProgramUniform>& ShaderCode::getProgramUniforms(const std::string& name)
    {
        // add all shader uniforms if list is empty
        if (mUniformsMap.empty()) {
            mUniformsMap["PosColorVerShader2D"] = gPosColorUniforms2D;
            //mUniformsMap["PosColorUVVerShader2D"] = gPosColorUVUniforms2D;
            //mUniformsMap["PosColorVerShader3D"] = gPosColorUniforms3D;
            //mUniformsMap["PosColorUVVerShader3D"] = gPosColorUVUniforms3D;
        }
        return mUniformsMap[name];
    }
}