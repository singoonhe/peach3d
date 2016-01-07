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
    // vertex shader
    const char* gPosColorVerShader2D = STRINGIFY(
    \n#ifdef PD_LEVEL_GL3\n
    uniform GlobalUnifroms {
        vec4 pd_viewRect;
    };
    in vec2 pd_vertex;
    in vec4 pd_showRect;   /* Widget position under anchor and size.*/
    in vec3 pd_anRot;      /* Widget anchor/rotate.*/
    in vec4 pd_patShowRect;/* Widget parent position under anchor and size.*/
    in vec3 pd_patAnRot;   /* Widget parent anchor/rotate.*/
    in vec4 pd_diffuse;
    out vec4 f_patShowRect;
    out vec3 f_patAnRot;
    out vec4 f_diffuse;
    \n#else\n
    attribute vec2 pd_vertex;
    uniform vec4 pd_viewRect;
    uniform vec4 pd_showRect;   /* Widget position under anchor and size.*/
    uniform vec3 pd_anRot;      /* Widget anchor/rotate.*/
    uniform vec4 pd_patShowRect;/* Widget parent position under anchor and size.*/
    uniform vec3 pd_patAnRot;   /* Widget parent anchor/rotate.*/
    uniform vec4 pd_diffuse;
    varying vec4 f_patShowRect;
    varying vec3 f_patAnRot;
    varying vec4 f_diffuse;
    \n#endif\n
    
    void main(void)
    {
        gl_Position = convertPosition2(pd_viewRect, pd_vertex, pd_showRect, pd_anRot);
        f_patShowRect = pd_patShowRect;
        f_patAnRot = pd_patAnRot;
        f_diffuse = pd_diffuse;
    });
    
    // fragment shader
    const char* gPosColorFragShader2D = STRINGIFY(
    \n#ifdef PD_LEVEL_GL3\n
    in vec4 f_patShowRect;
    in vec3 f_patAnRot;
    in vec4 f_diffuse;
    out vec4 out_FragColor;
    \n#else\n
        varying vec4 f_patShowRect;
    varying vec3 f_patAnRot;
    varying vec4 f_diffuse;
    \n#endif\n
                                                  
    void main(void)
    {
        clipFragment(f_patShowRect, f_patAnRot);
        \n#ifdef PD_LEVEL_GL3\n
        out_FragColor = f_diffuse;
        \n#else\n
        gl_FragColor = f_diffuse;
        \n#endif\n
    });
    
    std::vector<ProgramUniform> gPosColorUniforms2D = {ProgramUniform("pd_showRect", UniformDataType::eVector4),
        ProgramUniform("pd_anRot", UniformDataType::eVector3),
        ProgramUniform("pd_patShowRect", UniformDataType::eVector4),
        ProgramUniform("pd_patAnRot", UniformDataType::eVector3),
        ProgramUniform("pd_diffuse", UniformDataType::eVector4)};
}

#endif // PD_POSCOLOR_SHADER_2D_H
