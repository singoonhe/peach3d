//
//  Peach3DShaderCode.cpp
//  Peach3DLib
//
//  Created by singoon.he on 7/1/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#include "Peach3DShaderCode.h"
#include "Peach3DUtils.h"
#include "Peach3DIObject.h"

namespace Peach3D
{
    std::map<UniformDataType, uint> ShaderCode::mUniformsBitsMap;
    std::map<std::string, UniformNameType> ShaderCode::mUniformsNamesMap;
    
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
            mUniformsNamesMap["pd_shininess"] = UniformNameType::eMatShininess;
            mUniformsNamesMap["pd_ambient"] = UniformNameType::eMatAmbient;
            
            mUniformsNamesMap["pd_lType"] = UniformNameType::eLightType;
            mUniformsNamesMap["pd_lPosition"] = UniformNameType::eLightPos;
            mUniformsNamesMap["pd_lDirection"] = UniformNameType::eLightDir;
            mUniformsNamesMap["pd_lAttenuate"] = UniformNameType::eLightAtten;
            mUniformsNamesMap["pd_lSpotExtend"] = UniformNameType::eLightSpotExt;
            mUniformsNamesMap["pd_lAmbient"] = UniformNameType::eLightAmbient;
            mUniformsNamesMap["pd_lColor"] = UniformNameType::eLightColor;
            mUniformsNamesMap["pd_eyeDir"] = UniformNameType::eEyeDir;
        }
        return mUniformsNamesMap[name];
    }
        
    std::string ShaderCode::getNameOfProgramFeature(bool isVertex, const PresetProgramFeatures& feature)
    {
        return Utils::formatString("V%d|PT%d|TUV%d|LC%d", isVertex, feature.isPoint3, feature.isTexUV, feature.lightsCount);
    }
    
    uint ShaderCode::getVerTypeOfProgramFeature(const PresetProgramFeatures& feature)
    {
        uint verType = 0;
        if (feature.isPoint3) {
            verType = VertexType::Point3;
        }
        else {
            verType = VertexType::Point2;
        }
        if (feature.isTexUV) {
            verType = verType | VertexType::UV;
        }
        if (feature.lightsCount > 0) {
            verType = verType | VertexType::Normal;
        }
        return verType;
    }
}
