//
//  Peach3DShaderCode.cpp
//  Peach3DLib
//
//  Created by singoon.he on 7/1/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#include "Peach3DShaderCode.h"

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
        }
        return mUniformsNamesMap[name];
    }
    
    void ShaderCode::freeShaderCodeData()
    {
        for (auto iter = mShaderMap.begin(); iter != mShaderMap.end(); ++iter) {
            if (iter->second.size > 0) {
                free(iter->second.data);
            }
        }
        mShaderMap.clear();
    }

    ShaderCodeData ShaderCode::generateShaderCodeData(const char* code1, const char* code2)
    {
        ulong code1Len = strlen(code1), code2Len = strlen(code2);
        char* shaderData = (char*)malloc(code1Len + code2Len + 1);
        memcpy(shaderData, code1, code1Len);
        if (code2Len > 0) {
            memcpy(shaderData + code1Len, code2, code2Len);
        }
        return ShaderCodeData(shaderData, int(code1Len + code2Len));
    }
}
