//
//  pd2PosColorShader3D.h
//  Peach3DLib
//
//  Created by singoon.he on 10/28/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#ifndef PD2_POSCOLOR_SHADER_3D_H
#define PD2_POSCOLOR_SHADER_3D_H

#include "Peach3DShaderCode.h"

namespace Peach3D
{
    // vertex shader
    const char* g2PosColorVerShader3D = "attribute vec3 pd_vertex; \n"
    "uniform mat4 pd_projMatrix; \n"
    "uniform mat4 pd_viewMatrix; \n"
    "uniform mat4 pd_modelMatrix; \n"
    "uniform vec4 pd_diffuse; \n"
    "varying vec4 f_diffuse; \n"
    "void main(void) \n"
    "{ \n"
    "   mat4 mvpMatrix = pd_projMatrix * pd_viewMatrix * pd_modelMatrix; \n"
    "   gl_Position = mvpMatrix * vec4(pd_vertex, 1.0); \n"
    "   f_diffuse = pd_diffuse; \n"
    "}";
    // fragment shader
    const char* g2PosColorFragShader3D = "varying vec4 f_diffuse; \n"
    "void main(void) \n"
    "{ \n"
    "   gl_FragColor = f_diffuse; \n"
    "}";
    
    const std::vector<ProgramUniform> g2PosColorUniforms3D = {ProgramUniform("pd_projMatrix", UniformDataType::eMatrix4),
        ProgramUniform("pd_viewMatrix", UniformDataType::eMatrix4),
        ProgramUniform("pd_modelMatrix", UniformDataType::eMatrix4),
        ProgramUniform("pd_diffuse", UniformDataType::eVector4)};
}

#endif // PD2_POSCOLOR_SHADER_3D_H
