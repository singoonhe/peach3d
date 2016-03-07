//
//  Peach3DShaderCodeGL.cpp
//  Peach3DLib
//
//  Created by singoon.he on 7/1/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#include "Peach3DShaderCode.h"
#include "Peach3DIPlatform.h"
#include "Peach3DUtils.h"
#include "shader/pdShaderCode2D.h"
#include "shader/pdShaderCode3D.h"

namespace Peach3D
{
    const std::string& ShaderCode::getShaderCode(bool isVertex, const PresetProgramFeatures& feature)
    {
        auto featureStr = ShaderCode::getNameOfProgramFeature(isVertex, feature);
        if (mShaderMap.find(featureStr) == mShaderMap.end()) {
            std::string shaderPreStr;
            if (feature.isTexUV) {
                shaderPreStr += "#define PD_ENABLE_TEXUV\n";
            }
            if (feature.lightsCount > 0) {
                shaderPreStr += "#define PD_ENABLE_LIGHT\n";
                shaderPreStr += Utils::formatString("#define PD_LIGHT_COUNT %d\n", feature.lightsCount);
            }
            if (feature.isPoint3) {
                shaderPreStr += isVertex ? gVerShaderCode3D : gFragShaderCode3D;
            }
            else {
                shaderPreStr += isVertex ? gCommonVertexFunc2D : gCommonFragClipFunc2D;
                shaderPreStr += isVertex ? gVerShaderCode2D : gFragShaderCode2D;
            }
            mShaderMap[featureStr] = shaderPreStr;
//            if (feature.lightsCount > 0) {
                printf("shader code:\n%s\n\n\n", mShaderMap[featureStr].c_str());
//            }
        }
        return mShaderMap[featureStr];
    }
    
    const std::vector<ProgramUniform>& ShaderCode::getProgramUniforms(const PresetProgramFeatures& feature)
    {
        auto featureStr = ShaderCode::getNameOfProgramFeature(true, feature);
        if (mUniformsMap.find(featureStr) == mUniformsMap.end()) {
            std::vector<ProgramUniform> uniforms;
            if (feature.isPoint3) {
                if (PD_RENDERLEVEL() == RenderFeatureLevel::eGL2) {
                    uniforms.push_back(ProgramUniform("pd_projMatrix", UniformDataType::eMatrix4));
                    uniforms.push_back(ProgramUniform("pd_viewMatrix", UniformDataType::eMatrix4));
                    if (feature.lightsCount > 0) {
                        uniforms.push_back(ProgramUniform("pd_lType", UniformDataType::eFloat));
                        uniforms.push_back(ProgramUniform("pd_lPosition", UniformDataType::eVector3));
                        uniforms.push_back(ProgramUniform("pd_lDirection", UniformDataType::eVector3));
                        uniforms.push_back(ProgramUniform("pd_lAttenuate", UniformDataType::eVector3));
                        uniforms.push_back(ProgramUniform("pd_lSpotExtend", UniformDataType::eVector2));
                        uniforms.push_back(ProgramUniform("pd_lAmbient", UniformDataType::eVector3));
                        uniforms.push_back(ProgramUniform("pd_lColor", UniformDataType::eVector3));
                        uniforms.push_back(ProgramUniform("pd_eyeDir", UniformDataType::eVector3));
                    }
                }
                uniforms.push_back(ProgramUniform("pd_modelMatrix", UniformDataType::eMatrix4));
                uniforms.push_back(ProgramUniform("pd_diffuse", UniformDataType::eVector4));
                if (feature.lightsCount > 0) {
                    uniforms.push_back(ProgramUniform("pd_normalMatrix", UniformDataType::eMatrix4));
                }
            }
            else {
                if (PD_RENDERLEVEL() == RenderFeatureLevel::eGL2) {
                    uniforms.push_back(ProgramUniform("pd_viewRect", UniformDataType::eVector4));
                }
                uniforms.push_back(ProgramUniform("pd_showRect", UniformDataType::eVector4));
                uniforms.push_back(ProgramUniform("pd_anRot", UniformDataType::eVector3));
                uniforms.push_back(ProgramUniform("pd_patShowRect", UniformDataType::eVector4));
                uniforms.push_back(ProgramUniform("pd_patAnRot", UniformDataType::eVector3));
                uniforms.push_back(ProgramUniform("pd_diffuse", UniformDataType::eVector4));
                if (feature.isTexUV) {
                    uniforms.push_back(ProgramUniform("pd_uvRect", UniformDataType::eVector4));
                    uniforms.push_back(ProgramUniform("pd_texEffect", UniformDataType::eVector3));
                }
            }
            mUniformsMap[featureStr] = uniforms;
        }
        return mUniformsMap[featureStr];
    }
}