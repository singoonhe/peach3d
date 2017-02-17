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
    const std::string& ShaderCode::getShaderCode(bool isVertex, const ProgramFeatureMap& feature)
    {
        auto featureStr = ShaderCode::getNameOfProgramFeature(isVertex, feature);
        if (mShaderMap.find(featureStr) == mShaderMap.end()) {
            std::string shaderPreStr;
            // particle have no UV attribute
            auto findIter = feature.find(PROGRAM_FEATURE_UV);
            if (findIter!=feature.end() && findIter->second > 0) {
                shaderPreStr += "#define PD_ENABLE_TEXUV\n";
            }
            findIter = feature.find(PROGRAM_FEATURE_LIGHT);
            if (findIter!=feature.end() && findIter->second > 0) {
                shaderPreStr += "#define PD_ENABLE_LIGHT\n";
                shaderPreStr += Utils::formatString("#define PD_LIGHT_COUNT %d\n", findIter->second);
                
                findIter = feature.find(PROGRAM_FEATURE_SHADOW);
                if (findIter!=feature.end() && findIter->second > 0) {
                    shaderPreStr += Utils::formatString("#define PD_SHADOW_COUNT %d\n", findIter->second);
                }
            }
            findIter = feature.find(PROGRAM_FEATURE_BONE);
            if (findIter!=feature.end() && findIter->second > 0) {
                shaderPreStr += Utils::formatString("#define PD_ENABLE_SKELETON %d\n", findIter->second);
            }
            findIter = feature.find(PROGRAM_FEATURE_TERRAIN);
            if (findIter!=feature.end() && findIter->second > 0) {
                shaderPreStr += Utils::formatString("#define PD_ENABLE_TERRAIN %d\n", findIter->second);
            }
            findIter = feature.find(PROGRAM_FEATURE_POINT3);
            auto findParIter = feature.find(PROGRAM_FEATURE_PARTICLE);
            if (findIter!=feature.end() && findIter->second > 0) {
                if (findParIter!=feature.end() && findParIter->second > 0) {
                    // choose 3D particle program
                    shaderPreStr += isVertex ? gVerParticleShaderCode3D : gFragParticleShaderCode3D;
                }
                else {
                    // choose 3D node program
                    if (PD_RENDERLEVEL() == RenderFeatureLevel::eGL3) {
                        shaderPreStr += isVertex ? gVerGL3ShaderCode3D : gFragGL3ShaderCode3D;
                    }
                    else {
                        shaderPreStr += isVertex ? gVerGL2ShaderCode3D : gFragGL2ShaderCode3D;
                    }
                }
            }
            else {
                if (findParIter!=feature.end() && findParIter->second > 0) {
                    // choose 2D particle program
                    shaderPreStr += isVertex ? gVerParticleShaderCode2D : gFragParticleShaderCode2D;
                }
                else {
                    // choose 2D node program
                    shaderPreStr += isVertex ? gCommonVertexFunc2D : gCommonFragClipFunc2D;
                    shaderPreStr += isVertex ? gVerShaderCode2D : gFragShaderCode2D;
                }
            }
            mShaderMap[featureStr] = shaderPreStr;
        }
        return mShaderMap[featureStr];
    }
    
    const std::vector<ProgramUniform>& ShaderCode::getProgramUniforms(const ProgramFeatureMap& feature)
    {
        auto featureStr = ShaderCode::getNameOfProgramFeature(true, feature);
        if (mUniformsMap.find(featureStr) == mUniformsMap.end()) {
            std::vector<ProgramUniform> uniforms;
            auto findIter = feature.find(PROGRAM_FEATURE_POINT3);
            if (findIter!=feature.end() && findIter->second > 0) {
                findIter = feature.find(PROGRAM_FEATURE_PARTICLE);
                if (findIter!=feature.end() && findIter->second > 0) {
                    uniforms.push_back(ProgramUniform("pd_projMatrix", UniformDataType::eMatrix4));
                    uniforms.push_back(ProgramUniform("pd_viewMatrix", UniformDataType::eMatrix4));
                    // support texture plist
                    uniforms.push_back(ProgramUniform("pd_uvRect", UniformDataType::eVector4));
                }
                else {
                    if (PD_RENDERLEVEL() == RenderFeatureLevel::eGL2) {
                        uniforms.push_back(ProgramUniform("pd_projMatrix", UniformDataType::eMatrix4));
                        uniforms.push_back(ProgramUniform("pd_viewMatrix", UniformDataType::eMatrix4));
                        findIter = feature.find(PROGRAM_FEATURE_LIGHT);
                        if (findIter!=feature.end() && findIter->second > 0) {
                            uniforms.insert(uniforms.end(), ShaderCode::mLightUniforms.begin(), ShaderCode::mLightUniforms.end());
                            
                            findIter = feature.find(PROGRAM_FEATURE_SHADOW);
                            if (findIter!=feature.end() && findIter->second > 0) {
                                uniforms.push_back(ProgramUniform("pd_shadowMatrix", UniformDataType::eMatrix4));
                            }
                        }
                        findIter = feature.find(PROGRAM_FEATURE_BONE);
                        if (findIter!=feature.end() && findIter->second > 0) {
                            uniforms.push_back(ProgramUniform("pd_boneMatrix", UniformDataType::eVector4));
                        }
                    }
                    uniforms.push_back(ProgramUniform("pd_modelMatrix", UniformDataType::eMatrix4));
                    uniforms.push_back(ProgramUniform("pd_diffuse", UniformDataType::eVector4));
                    findIter = feature.find(PROGRAM_FEATURE_LIGHT);
                    if (findIter!=feature.end() && findIter->second > 0) {
                        uniforms.push_back(ProgramUniform("pd_normalMatrix", UniformDataType::eMatrix4));
                        uniforms.push_back(ProgramUniform("pd_ambient", UniformDataType::eVector3));
                        // specular included shininess
                        uniforms.push_back(ProgramUniform("pd_specular", UniformDataType::eVector4));
                        uniforms.push_back(ProgramUniform("pd_emissive", UniformDataType::eVector3));
                    }
                }
            }
            else {
                // particle only use pd_viewRect
                findIter = feature.find(PROGRAM_FEATURE_PARTICLE);
                if (PD_RENDERLEVEL() == RenderFeatureLevel::eGL2 || (findIter!=feature.end() && findIter->second > 0)) {
                    uniforms.push_back(ProgramUniform("pd_viewRect", UniformDataType::eVector4));
                    if ((findIter!=feature.end() && findIter->second > 0)) {
                        // support texture plist
                        uniforms.push_back(ProgramUniform("pd_uvRect", UniformDataType::eVector4));
                    }
                }
                if (!(findIter!=feature.end() && findIter->second > 0)) {
                    uniforms.push_back(ProgramUniform("pd_showRect", UniformDataType::eVector4));
                    uniforms.push_back(ProgramUniform("pd_anRot", UniformDataType::eVector3));
                    uniforms.push_back(ProgramUniform("pd_patShowRect", UniformDataType::eVector4));
                    uniforms.push_back(ProgramUniform("pd_patAnRot", UniformDataType::eVector3));
                    uniforms.push_back(ProgramUniform("pd_diffuse", UniformDataType::eVector4));
                    findIter = feature.find(PROGRAM_FEATURE_UV);
                    if (findIter!=feature.end() && findIter->second > 0) {
                        uniforms.push_back(ProgramUniform("pd_uvRect", UniformDataType::eVector4));
                        uniforms.push_back(ProgramUniform("pd_texEffect", UniformDataType::eVector3));
                    }
                }
            }
            mUniformsMap[featureStr] = uniforms;
        }
        return mUniformsMap[featureStr];
    }
}
