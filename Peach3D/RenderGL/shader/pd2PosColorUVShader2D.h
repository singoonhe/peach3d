//
//  pd2PosColorUVShader2D.h
//  Peach3DLib
//
//  Created by singoon.he on 10/28/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#ifndef PD2_POSCOLORUV_SHADER_2D_H
#define PD2_POSCOLORUV_SHADER_2D_H

#include "Peach3DShaderCode.h"

namespace Peach3D
{
    // vertex shader
    const char* g2PosColorUVVerShader2D = "attribute vec2 pd_vertex; \n"
    "uniform vec4 pd_viewRect; \n"
    "uniform vec4 pd_showRect;   /* Widget position under anchor and size.*/ \n"
    "uniform vec3 pd_anRot;      /* Widget anchor/rotate.*/ \n"
    "uniform vec4 pd_patShowRect;/* Widget parent position under anchor and size.*/ \n"
    "uniform vec3 pd_patAnRot;   /* Widget parent anchor/rotate.*/ \n"
    "uniform vec4 pd_diffuse; \n"
    "uniform vec4 pd_uvRect; \n"
    "uniform vec3 pd_texEffect; /* Sprite scale negative and gray effect. */ \n"
    "varying vec4 f_patShowRect; \n"
    "varying vec3 f_patAnRot; \n"
    "varying vec4 f_diffuse; \n"
    "varying vec2 f_uv; \n"
    "varying float f_gray; \n"
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
    const char* g2PosColorUVFragShader2D = "varying vec4 f_patShowRect; \n"
    "varying vec3 f_patAnRot; \n"
    "varying vec4 f_diffuse; \n"
    "varying vec2 f_uv; \n"
    "varying float f_gray; \n"
    "uniform sampler2D pd_texture0; \n"
    "void main(void) \n"
    "{ \n"
    "   clipFragment(f_patShowRect, f_patAnRot); \n"
    "   vec4 tecColor = texture2D(pd_texture0, f_uv) * f_diffuse; \n"
    "   if (f_gray > 0.0001) { \n"
    "       float grey = tecColor.r*0.299 + tecColor.g*0.587 + tecColor.b*0.114; \n"
    "       gl_FragColor = vec4(grey, grey, grey, tecColor.a); \n"
    "   } \n"
    "   else { \n"
    "       gl_FragColor = tecColor; \n"
    "   } \n"
    "}";
    
    const std::vector<ProgramUniform> g2PosColorUVUniforms2D = {ProgramUniform("pd_viewRect", UniformDataType::eVector4),
        ProgramUniform("pd_showRect", UniformDataType::eVector4),
        ProgramUniform("pd_anRot", UniformDataType::eVector3),
        ProgramUniform("pd_patShowRect", UniformDataType::eVector4),
        ProgramUniform("pd_patAnRot", UniformDataType::eVector3),
        ProgramUniform("pd_diffuse", UniformDataType::eVector4),
        ProgramUniform("pd_uvRect", UniformDataType::eVector4),
        ProgramUniform("pd_texEffect", UniformDataType::eVector3)};
}

#endif // PD2_POSCOLORUV_SHADER_2D_H
