//
//  pd3PosColorUVShader2D.h
//  Peach3DLib
//
//  Created by singoon.he on 10/28/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#ifndef PD3_POSCOLORUV_SHADER_2D_H
#define PD3_POSCOLORUV_SHADER_2D_H

#include "Peach3DShaderCode.h"

namespace Peach3D
{
    // vertex shader
    const char* g3PosColorUVVerShader2D = "uniform GlobalUnifroms { \n"
    "   vec4 pd_viewRect; \n"
    "}; \n"
    "in vec2 pd_vertex; \n"
    "in vec4 pd_showRect;   /* Widget position under anchor and size.*/ \n"
    "in vec3 pd_anRot;      /* Widget anchor/rotate.*/ \n"
    "in vec4 pd_patShowRect;/* Widget parent position under anchor and size.*/ \n"
    "in vec3 pd_patAnRot;   /* Widget parent anchor/rotate.*/ \n"
    "in vec4 pd_diffuse; \n"
    "in vec4 pd_uvRect; \n"
    "in vec3 pd_texEffect; /* Sprite scale negative and gray effect. */ \n"
    "out vec4 f_patShowRect; \n"
    "out vec3 f_patAnRot; \n"
    "out vec4 f_diffuse; \n"
    "out vec2 f_uv; \n"
    "out float f_gray; \n"
    "void main(void) \n"
    "{ \n"
    "   gl_Position = convertPosition2(pd_viewRect, pd_vertex, pd_showRect, pd_anRot); \n"
    "   f_patShowRect = pd_patShowRect; \n"
    "   f_patAnRot = pd_patAnRot; \n"
    "   f_diffuse = pd_diffuse; \n"
    "   float uvX = (pd_vertex.x * pd_texEffect.x + 1.0) * pd_uvRect.z / 2.0 + pd_uvRect.x; \n"
    "   float uvY = (pd_vertex.y * pd_texEffect.y + 1.0) * pd_uvRect.w / 2.0 + pd_uvRect.y; \n"
    "   f_uv = vec2(uvX, 1.0 -uvY); \n"
    "   f_gray = pd_texEffect.z; \n"
    "}";
    // fragment shader
    const char* g3PosColorUVFragShader2D = "in vec4 f_patShowRect; \n"
    "in vec3 f_patAnRot; \n"
    "in vec4 f_diffuse; \n"
    "in vec2 f_uv; \n"
    "in float f_gray; \n"
    "uniform sampler2D pd_texture0; \n"
    "out vec4 out_FragColor; \n"
    "void main(void) \n"
    "{ \n"
    "   clipFragment(f_patShowRect, f_patAnRot); \n"
    "   vec4 tecColor = texture(pd_texture0, f_uv) * f_diffuse; \n"
    "   if (f_gray > 0.0001) { \n"
    "       float grey = dot(tecColor.rgb, vec3(0.299, 0.587, 0.114)); \n"
    "       out_FragColor = vec4(grey, grey, grey, tecColor.a); \n"
    "   } \n"
    "   else { \n"
    "       out_FragColor = tecColor; \n"
    "   } \n"
    "}";
    
    const std::vector<ProgramUniform> g3PosColorUVUniforms2D = {ProgramUniform("pd_showRect", UniformDataType::eVector4),
        ProgramUniform("pd_anRot", UniformDataType::eVector3),
        ProgramUniform("pd_patShowRect", UniformDataType::eVector4),
        ProgramUniform("pd_patAnRot", UniformDataType::eVector3),
        ProgramUniform("pd_diffuse", UniformDataType::eVector4),
        ProgramUniform("pd_uvRect", UniformDataType::eVector4),
        ProgramUniform("pd_texEffect", UniformDataType::eVector3)};
}

#endif // PD3_POSCOLORUV_SHADER_2D_H
