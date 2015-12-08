//
//  pdShaderUniforms.h
//  Peach3DLib
//
//  Created by singoon.he on 10/28/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#ifndef PD_SHADER_UNIFORMS_H
#define PD_SHADER_UNIFORMS_H

#include "Peach3DShaderCode.h"

namespace Peach3D
{
    const std::vector<ProgramUniform> gPosColorUniforms2D = { ProgramUniform("pd_projMatrix", UniformDataType::eMatrix4),
        ProgramUniform("pd_viewMatrix", UniformDataType::eMatrix4),
        ProgramUniform("pd_modelMatrix", UniformDataType::eMatrix4),
        ProgramUniform("pd_diffuse", UniformDataType::eVector4) };

    const std::vector<ProgramUniform> gPosColorUVUniforms2D = { ProgramUniform("pd_projMatrix", UniformDataType::eMatrix4),
        ProgramUniform("pd_viewMatrix", UniformDataType::eMatrix4),
        ProgramUniform("pd_modelMatrix", UniformDataType::eMatrix4),
        ProgramUniform("pd_diffuse", UniformDataType::eVector4) };

    const std::vector<ProgramUniform> gPosColorUniforms3D = { ProgramUniform("pd_projMatrix", UniformDataType::eMatrix4),
        ProgramUniform("pd_viewMatrix", UniformDataType::eMatrix4),
        ProgramUniform("pd_modelMatrix", UniformDataType::eMatrix4),
        ProgramUniform("pd_diffuse", UniformDataType::eVector4) };
}

#endif // PD_SHADER_UNIFORMS_H
