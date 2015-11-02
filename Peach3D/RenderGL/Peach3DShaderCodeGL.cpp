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
#include "shader/pd2PosColorShader2D.h"
#include "shader/pd2PosColorShader3D.h"
#include "shader/pd2PosColorUVShader2D.h"
#include "shader/pd3PosColorShader2D.h"
#include "shader/pd3PosColorShader3D.h"
#include "shader/pd3PosColorUVShader2D.h"

namespace Peach3D
{
    std::map<std::string, std::string> ShaderCode::mShaderMap;
    std::map<std::string, std::vector<ProgramUniform>> ShaderCode::mUniformsMap;
    std::map<UniformDataType, uint> ShaderCode::mUniformsBitsMap;
    std::map<std::string, UniformNameType> ShaderCode::mUniformsNamesMap;
    
    const std::string& ShaderCode::getShaderCode(const std::string& name)
    {
        // add all shader code if list is empty
        if (mShaderMap.empty()) {
            if (PD_RENDERLEVEL() == RenderFeatureLevel::eGL3) {
                mShaderMap["PosColorVerShader2D"] = gCommonVertexFunc2D + g3PosColorVerShader2D;
                mShaderMap["PosColorFragShader2D"] = gCommonFragClipFunc2D + g3PosColorFragShader2D;
                mShaderMap["PosColorUVVerShader2D"] = gCommonVertexFunc2D + g3PosColorUVVerShader2D;
                mShaderMap["PosColorUVFragShader2D"] = gCommonFragClipFunc2D + g3PosColorUVFragShader2D;
                mShaderMap["PosColorVerShader3D"] = g3PosColorVerShader3D;
                mShaderMap["PosColorFragShader3D"] = g3PosColorFragShader3D;
            }
            else {
                mShaderMap["PosColorVerShader2D"] = gCommonVertexFunc2D + g2PosColorVerShader2D;
                mShaderMap["PosColorFragShader2D"] = gCommonFragClipFunc2D + g2PosColorFragShader2D;
                mShaderMap["PosColorUVVerShader2D"] = gCommonVertexFunc2D + g2PosColorUVVerShader2D;
                mShaderMap["PosColorUVFragShader2D"] = gCommonFragClipFunc2D + g2PosColorUVFragShader2D;
                mShaderMap["PosColorVerShader3D"] = g2PosColorVerShader3D;
                mShaderMap["PosColorFragShader3D"] = g2PosColorFragShader3D;
            }
        }
        return mShaderMap[name];
    }
    
    const std::vector<ProgramUniform>& ShaderCode::getProgramUniforms(const std::string& name)
    {
        // add all shader uniforms if list is empty
        if (mUniformsMap.empty()) {
            if (PD_RENDERLEVEL() == RenderFeatureLevel::eGL2) {
                mUniformsMap["PosColorVerShader2D"] = g2PosColorUniforms2D;
                mUniformsMap["PosColorUVVerShader2D"] = g2PosColorUVUniforms2D;
                mUniformsMap["PosColorVerShader3D"] = g2PosColorUniforms3D;
            }
            else {
                mUniformsMap["PosColorVerShader2D"] = g3PosColorUniforms2D;
                mUniformsMap["PosColorUVVerShader2D"] = g3PosColorUVUniforms2D;
                mUniformsMap["PosColorVerShader3D"] = g3PosColorUniforms3D;
            }
        }
        return mUniformsMap[name];
    }
    
    uint ShaderCode::getUniformFloatBits(UniformDataType type)
    {
        if (mUniformsBitsMap.empty()) {
            mUniformsBitsMap[UniformDataType::eFloat] = 1;
            mUniformsBitsMap[UniformDataType::eVector2] = 2;
            mUniformsBitsMap[UniformDataType::eVector3] = 3;
            mUniformsBitsMap[UniformDataType::eVector4] = 4;
            mUniformsBitsMap[UniformDataType::eMatrix4] = 16;
        }
        return mUniformsBitsMap[type];
    }
    
    UniformNameType ShaderCode::getUniformNameType(const std::string& name)
    {
        if (mUniformsNamesMap.empty()) {
            mUniformsNamesMap["pd_viewRect"] = UniformNameType::eViewRect;
            mUniformsNamesMap["pd_showRect"] = UniformNameType::eShowRect;
            mUniformsNamesMap["pd_anRot"] = UniformNameType::eAnRot;
            mUniformsNamesMap["pd_patShowRect"] = UniformNameType::ePatShowRect;
            mUniformsNamesMap["pd_patAnRot"] = UniformNameType::ePatAnRot;
            mUniformsNamesMap["pd_diffuse"] = UniformNameType::eDiffuse;
            mUniformsNamesMap["pd_uvRect"] = UniformNameType::eUVRect;
            mUniformsNamesMap["pd_texEffect"] = UniformNameType::eTexEffect;
            
            mUniformsNamesMap["pd_projMatrix"] = UniformNameType::eProjMatrix;
            mUniformsNamesMap["pd_viewMatrix"] = UniformNameType::eViewMatrix;
            mUniformsNamesMap["pd_modelMatrix"] = UniformNameType::eModelMatrix;
        }
        return mUniformsNamesMap[name];
    }
}