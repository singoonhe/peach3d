//
//  Peach3DShaderCodeGL.cpp
//  Peach3DLib
//
//  Created by singoon.he on 7/1/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#include "Peach3DShaderCode.h"
#include "Peach3DIPlatform.h"
#include "shader/pdShaderUniforms.h"
#include "Peach3DResourceManager.h"

namespace Peach3D
{
    const ShaderCodeData& ShaderCode::getShaderCode(const std::string& name)
    {
        // add all shader code if list is empty
        if (mShaderMap.empty()) {
            std::map<std::string, std::string> fileMap;
            fileMap["PosColorVerShader2D"] = "pdPosColorVert2D.cso";
            fileMap["PosColorFragShader2D"] = "pdPosColorPixel2D.cso";
            fileMap["PosColorUVVerShader2D"] = "pdPosColorVert2D.cso";
            fileMap["PosColorUVFragShader2D"] = "pdPosColorPixel2D.cso";
            fileMap["PosColorVerShader3D"] = "pdPosColorVert2D.cso";
            fileMap["PosColorFragShader3D"] = "pdPosColorPixel2D.cso";

            for (auto iter = fileMap.begin(); iter != fileMap.end(); ++iter) {
                ulong texLength = 0;
                // get compiled shader file data
                uchar *texData = ResourceManager::getSingleton().getFileData(("Peach3D/"+ iter->second).c_str(), &texLength);
                mShaderMap[iter->first] = ShaderCodeData((char*)texData, (int)texLength);
            }
        }
        return mShaderMap[name];
    }
    
    const std::vector<ProgramUniform>& ShaderCode::getProgramUniforms(const std::string& name)
    {
        // add all shader uniforms if list is empty
        if (mUniformsMap.empty()) {
            mUniformsMap["PosColorVerShader2D"] = gPosColorUniforms2D;
            mUniformsMap["PosColorUVVerShader2D"] = gPosColorUVUniforms2D;
            mUniformsMap["PosColorVerShader3D"] = gPosColorUniforms3D;
        }
        return mUniformsMap[name];
    }
}