//
//  pd2PosColorShader2D.h
//  Peach3DLib
//
//  Created by singoon.he on 10/28/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#ifndef PD2_POSCOLOR_SHADER_2D_H
#define PD2_POSCOLOR_SHADER_2D_H

#include "Peach3DShaderCode.h"

namespace Peach3D
{
    //const char* gPosColorVerShader2D = "struct VertexShaderInput { \n"
    //    "\n"
    //    "   float3 pos : POSITION; \n"
    //    "   float3 color : COLOR0; \n"
    //    "}; \n
    //    struct PixelShaderInput \n"
    //    "{ \n"
    //    "	  float4 pos : SV_POSITION; \n"
    //    "   float3 color : COLOR0; \n"
    //    "}; \n
    //    "PixelShaderInput main(VertexShaderInput input) \n"
    //    "uniform vec4 pd_diffuse; \n"
    //    "varying vec4 f_patShowRect; \n"
    //    "varying vec3 f_patAnRot; \n"
    //    "varying vec4 f_diffuse; \n"
    //    "void main(void) \n"
    //    "{ \n"
    //    "   gl_Position = convertPosition2(pd_viewRect, pd_vertex, pd_showRect, pd_anRot); \n"
    //    "   f_patShowRect = pd_patShowRect; \n"
    //    "   f_patAnRot = pd_patAnRot; \n"
    //    "   f_diffuse = pd_diffuse; \n"
    //    "}";
    // vertex shader
    const char* g2PosColorVerShader2D = "attribute vec2 pd_vertex; \n"
    "uniform vec4 pd_viewRect; \n"
    "uniform vec4 pd_showRect;   /* Widget position under anchor and size.*/ \n"
    "uniform vec3 pd_anRot;      /* Widget anchor/rotate.*/ \n"
    "uniform vec4 pd_patShowRect;/* Widget parent position under anchor and size.*/ \n"
    "uniform vec3 pd_patAnRot;   /* Widget parent anchor/rotate.*/ \n"
    "uniform vec4 pd_diffuse; \n"
    "varying vec4 f_patShowRect; \n"
    "varying vec3 f_patAnRot; \n"
    "varying vec4 f_diffuse; \n"
    "void main(void) \n"
    "{ \n"
    "   gl_Position = convertPosition2(pd_viewRect, pd_vertex, pd_showRect, pd_anRot); \n"
    "   f_patShowRect = pd_patShowRect; \n"
    "   f_patAnRot = pd_patAnRot; \n"
    "   f_diffuse = pd_diffuse; \n"
    "}";
    // fragment shader
    const char* g2PosColorFragShader2D = "varying vec4 f_patShowRect; \n"
    "varying vec3 f_patAnRot; \n"
    "varying vec4 f_diffuse; \n"
    "void main(void) \n"
    "{ \n"
    "   clipFragment(f_patShowRect, f_patAnRot); \n"
    "   gl_FragColor = f_diffuse; \n"
    "}";
    
    const std::vector<ProgramUniform> g2PosColorUniforms2D = {ProgramUniform("pd_viewRect", UniformDataType::eVector4),
        ProgramUniform("pd_showRect", UniformDataType::eVector4),
        ProgramUniform("pd_anRot", UniformDataType::eVector3),
        ProgramUniform("pd_patShowRect", UniformDataType::eVector4),
        ProgramUniform("pd_patAnRot", UniformDataType::eVector3),
        ProgramUniform("pd_diffuse", UniformDataType::eVector4)};
}

#endif // PD2_POSCOLOR_SHADER_2D_H
