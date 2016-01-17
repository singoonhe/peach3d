//
//  pdPosColorShader2D.h
//  Peach3DLib
//
//  Created by singoon.he on 10/28/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#ifndef PD_POSCOLOR_SHADER_2D_H
#define PD_POSCOLOR_SHADER_2D_H

#include "Peach3DShaderCode.h"

namespace Peach3D
{
    // vertex shader and pixel shader
    const char* gPosColorShader2D = STRINGIFY(
    struct VSInput
    {
        float2 pos : pd_vertex;
    };
    struct PSInput
    {
        float4 pos : SV_POSITION;
    };

    PSInput VSMain(VSInput input)
    {
        PSInput output;
        output.pos = float4(input.pos, 0.0f, 1.0f);

        return output;
    }
    float4 PSMain(PSInput input) : SV_TARGET
    {
        return float4(1.0f, 0.0f, 0.0f, 1.0f);
    });
    
    //std::vector<ProgramUniform> gPosColorUniforms2D = {ProgramUniform("pd_showRect", UniformDataType::eVector4),
    //    ProgramUniform("pd_anRot", UniformDataType::eVector3),
    //    ProgramUniform("pd_patShowRect", UniformDataType::eVector4),
    //    ProgramUniform("pd_patAnRot", UniformDataType::eVector3),
    //    ProgramUniform("pd_diffuse", UniformDataType::eVector4)};
    std::vector<ProgramUniform> gPosColorUniforms2D = {};
}

#endif // PD_POSCOLOR_SHADER_2D_H
