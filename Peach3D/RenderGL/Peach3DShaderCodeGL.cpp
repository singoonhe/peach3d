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
            if (ShaderCode::isContainTypeFeature(feature, PROGRAM_FEATURE_UV)) {
                shaderPreStr += "#define PD_ENABLE_TEXUV\n";
            }
            auto lightCount = ShaderCode::getCountOfTypeFeature(feature, PROGRAM_FEATURE_LIGHT);
            if (lightCount > 0) {
                shaderPreStr += "#define PD_ENABLE_LIGHT\n";
                shaderPreStr += Utils::formatString("#define PD_LIGHT_COUNT %d\n", lightCount);
                
                auto shadowCount = ShaderCode::getCountOfTypeFeature(feature, PROGRAM_FEATURE_SHADOW);
                if (shadowCount > 0) {
                    shaderPreStr += Utils::formatString("#define PD_SHADOW_COUNT %d\n", shadowCount);
                }
            }
            auto bonesCount = ShaderCode::getCountOfTypeFeature(feature, PROGRAM_FEATURE_BONE);
            if (bonesCount > 0) {
                shaderPreStr += Utils::formatString("#define PD_ENABLE_SKELETON %d\n", bonesCount);
            }
            auto terrCount = ShaderCode::getCountOfTypeFeature(feature, PROGRAM_FEATURE_TERRAIN);
            if (terrCount > 0) {
                shaderPreStr += Utils::formatString("#define PD_ENABLE_TERRAIN %d\n", terrCount);
            }
            
            auto isParticle = ShaderCode::isContainTypeFeature(feature, PROGRAM_FEATURE_PARTICLE);
            if (ShaderCode::isContainTypeFeature(feature, PROGRAM_FEATURE_POINT3)) {
                if (isParticle) {
                    // choose 3D particle program
                    shaderPreStr += isVertex ? gVerParticleShaderCode3D : gFragParticleShaderCode3D;
                }
                else {
                    // choose 3D node program
                    if (PD_RENDERLEVEL()==RenderFeatureLevel::eGL2) {
                        shaderPreStr += isVertex ? gVerGL2ShaderCode3D : gFragGL2ShaderCode3D;
                    }
                    else {
                        shaderPreStr += isVertex ? gVerGL3ShaderCode3D : gFragGL3ShaderCode3D;
                    }
                }
            }
            else {
                if (isParticle) {
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
            if (ShaderCode::isContainTypeFeature(feature, PROGRAM_FEATURE_POINT3)) {
                if (ShaderCode::isContainTypeFeature(feature, PROGRAM_FEATURE_PARTICLE)) {
                    uniforms.push_back(ProgramUniform("pd_projMatrix", UniformDataType::eMatrix4));
                    uniforms.push_back(ProgramUniform("pd_viewMatrix", UniformDataType::eMatrix4));
                    // support texture plist
                    uniforms.push_back(ProgramUniform("pd_uvRect", UniformDataType::eVector4));
                }
                else {
                    if (PD_RENDERLEVEL()==RenderFeatureLevel::eGL2) {
                        uniforms.push_back(ProgramUniform("pd_projMatrix", UniformDataType::eMatrix4));
                        uniforms.push_back(ProgramUniform("pd_viewMatrix", UniformDataType::eMatrix4));
                        if (ShaderCode::isContainTypeFeature(feature, PROGRAM_FEATURE_LIGHT)) {
                            uniforms.insert(uniforms.end(), ShaderCode::mLightUniforms.begin(), ShaderCode::mLightUniforms.end());
                            
                            if (ShaderCode::isContainTypeFeature(feature, PROGRAM_FEATURE_SHADOW)) {
                                uniforms.push_back(ProgramUniform("pd_shadowMatrix", UniformDataType::eMatrix4));
                            }
                        }
                        if (ShaderCode::isContainTypeFeature(feature, PROGRAM_FEATURE_BONE)) {
                            uniforms.push_back(ProgramUniform("pd_boneMatrix", UniformDataType::eVector4));
                        }
                        if (ShaderCode::isContainTypeFeature(feature, PROGRAM_FEATURE_TERRAIN)) {
                            uniforms.push_back(ProgramUniform("pd_detailSize", UniformDataType::eFloat));
                        }
                    }
                    uniforms.push_back(ProgramUniform("pd_modelMatrix", UniformDataType::eMatrix4));
                    uniforms.push_back(ProgramUniform("pd_diffuse", UniformDataType::eVector4));
                    if (ShaderCode::isContainTypeFeature(feature, PROGRAM_FEATURE_LIGHT)) {
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
                auto isParticle = ShaderCode::isContainTypeFeature(feature, PROGRAM_FEATURE_PARTICLE);
                if (PD_RENDERLEVEL()==RenderFeatureLevel::eGL2 || isParticle) {
                    uniforms.push_back(ProgramUniform("pd_viewRect", UniformDataType::eVector4));
                    if (isParticle) {
                        // support texture plist
                        uniforms.push_back(ProgramUniform("pd_uvRect", UniformDataType::eVector4));
                    }
                }
                if (!isParticle) {
                    uniforms.push_back(ProgramUniform("pd_showRect", UniformDataType::eVector4));
                    uniforms.push_back(ProgramUniform("pd_anRot", UniformDataType::eVector3));
                    uniforms.push_back(ProgramUniform("pd_patShowRect", UniformDataType::eVector4));
                    uniforms.push_back(ProgramUniform("pd_patAnRot", UniformDataType::eVector3));
                    uniforms.push_back(ProgramUniform("pd_diffuse", UniformDataType::eVector4));
                    if (ShaderCode::isContainTypeFeature(feature, PROGRAM_FEATURE_UV)) {
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
