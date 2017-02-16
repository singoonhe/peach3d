//
//  Peach3DShaderCode.cpp
//  Peach3DLib
//
//  Created by singoon.he on 7/1/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#include <sstream>
#include "Peach3DShaderCode.h"
#include "Peach3DUtils.h"
#include "Peach3DIObject.h"

namespace Peach3D
{
    std::map<UniformDataType, uint> ShaderCode::mUniformsBitsMap;
    std::map<std::string, UniformNameType> ShaderCode::mUniformsNamesMap;
    std::vector<ProgramUniform> ShaderCode::mLightUniforms = {ProgramUniform("pd_lTypeSpot", UniformDataType::eVector3),
        ProgramUniform("pd_lPosition", UniformDataType::eVector3),
        ProgramUniform("pd_lDirection", UniformDataType::eVector3),
        ProgramUniform("pd_lAttenuate", UniformDataType::eVector3),
        ProgramUniform("pd_lAmbient", UniformDataType::eVector3),
        ProgramUniform("pd_lColor", UniformDataType::eVector3),
        ProgramUniform("pd_eyeDir", UniformDataType::eVector3)};
    
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
            mUniformsNamesMap["pd_normalMatrix"] = UniformNameType::eNormalMatrix;
            mUniformsNamesMap["pd_emissive"] = UniformNameType::eMatEmissive;
            mUniformsNamesMap["pd_specular"] = UniformNameType::eMatSpecular;
            mUniformsNamesMap["pd_ambient"] = UniformNameType::eMatAmbient;
            
            mUniformsNamesMap["pd_lTypeSpot"] = UniformNameType::eLightTypeSpot;
            mUniformsNamesMap["pd_lPosition"] = UniformNameType::eLightPos;
            mUniformsNamesMap["pd_lDirection"] = UniformNameType::eLightDir;
            mUniformsNamesMap["pd_lAttenuate"] = UniformNameType::eLightAtten;
            mUniformsNamesMap["pd_lAmbient"] = UniformNameType::eLightAmbient;
            mUniformsNamesMap["pd_lColor"] = UniformNameType::eLightColor;
            mUniformsNamesMap["pd_eyeDir"] = UniformNameType::eEyeDir;
            
            mUniformsNamesMap["pd_shadowMatrix"] = UniformNameType::eShadowMatrix;
            mUniformsNamesMap["pd_boneMatrix"] = UniformNameType::eBoneMatrix;
        }
        return mUniformsNamesMap[name];
    }
        
    std::string ShaderCode::getNameOfProgramFeature(bool isVertex, const ProgramFeatureMap& feature)
    {
        // is vertex shader
        std::ostringstream retString;
        retString<<"V"<<(isVertex ? "1" : "0");
        // is point3
        retString<<"|PT";
        auto findIter = feature.find(PROGRAM_FEATURE_POINT3);
        retString<<((findIter!=feature.end() && findIter->second > 0) ? "1" : "0");
        // texture UV
        findIter = feature.find(PROGRAM_FEATURE_UV);
        if ((findIter!=feature.end() && findIter->second > 0)) {
            retString<<"|TUV1";
        }
        // light count
        findIter = feature.find(PROGRAM_FEATURE_LIGHT);
        if ((findIter!=feature.end() && findIter->second > 0)) {
            retString<<"|LC"<<findIter->second;
        }
        // shadow count
        findIter = feature.find(PROGRAM_FEATURE_SHADOW);
        if ((findIter!=feature.end() && findIter->second > 0)) {
            retString<<"|SC"<<findIter->second;
        }
        // bones count
        findIter = feature.find(PROGRAM_FEATURE_BONE);
        if ((findIter!=feature.end() && findIter->second > 0)) {
            retString<<"|SK"<<findIter->second;
        }
        // is particle
        findIter = feature.find(PROGRAM_FEATURE_PARTICLE);
        if ((findIter!=feature.end() && findIter->second > 0)) {
            retString<<"|PC1";
        }
        return retString.str();
    }
    
    uint ShaderCode::getVerTypeOfProgramFeature(const ProgramFeatureMap& feature)
    {
        auto findIter = feature.find(PROGRAM_FEATURE_POINT3);
        uint verType = 0;
        if (findIter != feature.end() && findIter->second > 0) {
            verType = VertexType::Point3;
        }
        else {
            verType = VertexType::Point2;
        }
        findIter = feature.find(PROGRAM_FEATURE_UV);
        if (findIter != feature.end() && findIter->second > 0) {
            verType = verType | VertexType::UV;
        }
        findIter = feature.find(PROGRAM_FEATURE_LIGHT);
        if (findIter != feature.end() && findIter->second > 0) {
            verType = verType | VertexType::Normal;
        }
        findIter = feature.find(PROGRAM_FEATURE_BONE);
        if (findIter != feature.end() && findIter->second > 0) {
            verType = verType | VertexType::BWidget;
            verType = verType | VertexType::BIndex;
        }
        findIter = feature.find(PROGRAM_FEATURE_PARTICLE);
        if (findIter != feature.end() && findIter->second > 0) {
            verType = verType | VertexType::PSprite;
        }
        return verType;
    }
}
