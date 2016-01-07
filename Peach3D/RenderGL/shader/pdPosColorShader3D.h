//
//  pdPosColorShader3D.h
//  Peach3DLib
//
//  Created by singoon.he on 10/28/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#ifndef PD_POSCOLOR_SHADER_3D_H
#define PD_POSCOLOR_SHADER_3D_H

#include "Peach3DShaderCode.h"

namespace Peach3D
{
    // vertex shader
    const char* gPosColorVerShader3D = STRINGIFY(
    \n#ifdef PD_LEVEL_GL3\n
    uniform GlobalUnifroms {
        mat4 pd_projMatrix;
        mat4 pd_viewMatrix;
    };
    in vec3 pd_vertex;
    in mat4 pd_modelMatrix;
    in vec4 pd_diffuse;
    out vec4 f_diffuse;
    \n#else\n
    attribute vec3 pd_vertex;
    uniform mat4 pd_projMatrix;
    uniform mat4 pd_viewMatrix;
    uniform mat4 pd_modelMatrix;
    uniform vec4 pd_diffuse;
    varying vec4 f_diffuse;
    \n#endif\n

    void main(void)
    {
        mat4 mvpMatrix = pd_projMatrix * pd_viewMatrix * pd_modelMatrix;
        gl_Position = mvpMatrix * vec4(pd_vertex, 1.0);
        f_diffuse = pd_diffuse;
    });
    
    // fragment shader
    const char* gPosColorFragShader3D = STRINGIFY(
    \n#ifdef PD_LEVEL_GL3\n
    in vec4 f_diffuse;
    out vec4 out_FragColor;
    \n#else\n
    varying vec4 f_diffuse;
    \n#endif\n
    
    void main(void)
    {
        \n#ifdef PD_LEVEL_GL3\n
        out_FragColor = f_diffuse;
        \n#else\n
        gl_FragColor = f_diffuse;
        \n#endif\n
    });
    
    std::vector<ProgramUniform> gPosColorUniforms3D = {ProgramUniform("pd_modelMatrix", UniformDataType::eMatrix4),
        ProgramUniform("pd_diffuse", UniformDataType::eVector4)};
}

#endif // PD_POSCOLOR_SHADER_3D_H
