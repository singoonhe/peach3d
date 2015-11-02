//
//  pd3PosColorShader3D.h
//  Peach3DLib
//
//  Created by singoon.he on 10/28/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#ifndef PD3_POSCOLOR_SHADER_3D_H
#define PD3_POSCOLOR_SHADER_3D_H

#include "Peach3DShaderCode.h"

namespace Peach3D
{
    // vertex shader
    const char* g3PosColorVerShader3D = "uniform GlobalUnifroms { \n"
    "   mat4 pd_projMatrix; \n"
    "   mat4 pd_viewMatrix; \n"
    "}; \n"
    "in vec3 pd_vertex; \n"
    "in mat4 pd_modelMatrix; \n"
    "void main(void) \n"
    "{ \n"
    "   mat4 mvpMatrix = pd_projMatrix * pd_viewMatrix * pd_modelMatrix; \n"
    "   gl_Position = mvpMatrix * vec4(pd_vertex, 1.0); \n"
    "}";
    // fragment shader
    const char* g3PosColorFragShader3D = "in vec4 f_diffuse; \n"
    "out vec4 out_FragColor; \n"
    "void main(void) \n"
    "{ \n"
    "   out_FragColor = f_diffuse; \n"
    "}";
    
    const std::vector<ProgramUniform> g3PosColorUniforms3D = {ProgramUniform("pd_modelMatrix", UniformDataType::eMatrix4),
        ProgramUniform("pd_diffuse", UniformDataType::eVector4)};
}

#endif // PD3_POSCOLOR_SHADER_3D_H
