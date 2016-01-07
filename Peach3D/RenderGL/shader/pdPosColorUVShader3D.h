//
//  pdPosColorUVShader3D.h
//  Peach3DLib
//
//  Created by singoon.he on 1/7/16.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#ifndef PD_POSCOLORUV_SHADER_3D_H
#define PD_POSCOLORUV_SHADER_3D_H

#include "Peach3DShaderCode.h"

namespace Peach3D
{
    // vertex shader
    const char* gPosColorUVVerShader3D = STRINGIFY(
    \n#ifdef PD_LEVEL_GL3\n
    uniform GlobalUnifroms {
        mat4 pd_projMatrix;
        mat4 pd_viewMatrix;
    };
    in vec3 pd_vertex;
    in vec2 pd_uv;
    in mat4 pd_modelMatrix;
    in vec4 pd_diffuse;
    out vec4 f_diffuse;
    out vec2 f_uv;
    \n#else\n
    attribute vec3 pd_vertex;
    attribute vec2 pd_uv;
    uniform mat4 pd_projMatrix;
    uniform mat4 pd_viewMatrix;
    uniform mat4 pd_modelMatrix;
    uniform vec4 pd_diffuse;
    varying vec4 f_diffuse;
    varying vec2 f_uv;
    \n#endif\n
    
    void main(void)
    {
        mat4 mvpMatrix = pd_projMatrix * pd_viewMatrix * pd_modelMatrix;
        gl_Position = mvpMatrix * vec4(pd_vertex, 1.0);
        f_diffuse = pd_diffuse;
        f_uv = pd_uv;
    });

    // fragment shader
    const char* gPosColorUVFragShader3D = STRINGIFY(
    \n#ifdef PD_LEVEL_GL3\n
    in vec4 f_diffuse;
    in vec2 f_uv;
    uniform sampler2D pd_texture0;  /* Texture must named "pd_texturex".*/
    out vec4 out_FragColor;
    \n#else\n
    varying vec4 f_diffuse;
    varying vec2 f_uv;
    uniform sampler2D pd_texture0;  /* Texture must named "pd_texturex".*/
    \n#endif\n
    
    void main(void)
    {
        \n#ifdef PD_LEVEL_GL3\n
        vec4 tex_color0 = texture( pd_texture0, out_uv );
        out_FragColor = mix(f_diffuse, tex_color0, tex_color0.a);;
        \n#else\n
        vec4 tex_color0 = texture2D( pd_texture0, out_uv );
        gl_FragColor = mix(f_diffuse, tex_color0, tex_color0.a);
        \n#endif\n
    });

    std::vector<ProgramUniform> gPosColorUVUniforms3D = {ProgramUniform("pd_modelMatrix", UniformDataType::eMatrix4),
        ProgramUniform("pd_diffuse", UniformDataType::eVector4)};
}

#endif // PD_POSCOLORUV_SHADER_3D_H
