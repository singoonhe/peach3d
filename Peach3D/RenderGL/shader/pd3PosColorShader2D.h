//
//  pd3PosColorShader2D.h
//  Peach3DLib
//
//  Created by singoon.he on 10/28/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#ifndef PD3_POSCOLOR_SHADER_2D_H
#define PD3_POSCOLOR_SHADER_2D_H

#include "Peach3DShaderCode.h"

namespace Peach3D
{
    // vertex shader
    const char* g3PosColorVerShader2D = "uniform GlobalUnifroms { \n"
    "   vec4 pd_viewRect; \n"
    "}; \n"
    "in vec2 pd_vertex; \n"
    "in vec4 pd_showRect;   /* Widget position under anchor and size.*/ \n"
    "in vec3 pd_anRot;      /* Widget anchor/rotate.*/ \n"
    "in vec4 pd_patShowRect;/* Widget parent position under anchor and size.*/ \n"
    "in vec3 pd_patAnRot;   /* Widget parent anchor/rotate.*/ \n"
    "in vec4 pd_diffuse; \n"
    "out vec4 f_patShowRect; \n"
    "out vec3 f_patAnRot; \n"
    "out vec4 f_diffuse; \n"
    "void main(void) \n"
    "{ \n"
    "   gl_Position = convertPosition2(pd_viewRect, pd_vertex, pd_showRect, pd_anRot); \n"
    "   f_patShowRect = pd_patShowRect; \n"
    "   f_patAnRot = pd_patAnRot; \n"
    "   f_diffuse = pd_diffuse; \n"
    "}";
    // fragment shader
    const char* g3PosColorFragShader2D = "in vec4 f_patShowRect; \n"
    "in vec3 f_patAnRot; \n"
    "in vec4 f_diffuse; \n"
    "out vec4 out_FragColor; \n"
    "void main(void) \n"
    "{ \n"
    "   clipFragment(f_patShowRect, f_patAnRot); \n"
    "   out_FragColor = f_diffuse; \n"
    "}";
    
    const std::vector<ProgramUniform> g3PosColorUniforms2D = {ProgramUniform("pd_showRect", UniformDataType::eVector4),
        ProgramUniform("pd_anRot", UniformDataType::eVector3),
        ProgramUniform("pd_patShowRect", UniformDataType::eVector4),
        ProgramUniform("pd_patAnRot", UniformDataType::eVector3),
        ProgramUniform("pd_diffuse", UniformDataType::eVector4)};
}

#endif // PD3_POSCOLOR_SHADER_2D_H
