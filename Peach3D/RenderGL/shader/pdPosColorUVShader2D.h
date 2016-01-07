//
//  pdPosColorUVShader2D.h
//  Peach3DLib
//
//  Created by singoon.he on 10/28/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#ifndef PD_POSCOLORUV_SHADER_2D_H
#define PD_POSCOLORUV_SHADER_2D_H

#include "Peach3DShaderCode.h"

namespace Peach3D
{
    // vertex shader
    const char* gPosColorUVVerShader2D = STRINGIFY(
    \n#ifdef PD_LEVEL_GL3\n
    uniform GlobalUnifroms {
        vec4 pd_viewRect;
    };
    in vec2 pd_vertex;
    in vec4 pd_showRect;    /* Widget position under anchor and size.*/
    in vec3 pd_anRot;       /* Widget anchor/rotate.*/
    in vec4 pd_patShowRect; /* Widget parent position under anchor and size.*/
    in vec3 pd_patAnRot;    /* Widget parent anchor/rotate.*/
    in vec4 pd_diffuse;
    in vec4 pd_uvRect;
    in vec3 pd_texEffect;   /* Sprite scale negative and gray effect. */
    out vec4 f_patShowRect;
    out vec3 f_patAnRot;
    out vec4 f_diffuse;
    out vec2 f_uv;
    out float f_gray;
    \n#else\n
    attribute vec2 pd_vertex;
    uniform vec4 pd_viewRect;
    uniform vec4 pd_showRect;   /* Widget position under anchor and size.*/
    uniform vec3 pd_anRot;      /* Widget anchor/rotate.*/
    uniform vec4 pd_patShowRect;/* Widget parent position under anchor and size.*/
    uniform vec3 pd_patAnRot;   /* Widget parent anchor/rotate.*/
    uniform vec4 pd_diffuse;
    uniform vec4 pd_uvRect;
    uniform vec3 pd_texEffect;  /* Sprite scale negative and gray effect. */
    varying vec4 f_patShowRect;
    varying vec3 f_patAnRot;
    varying vec4 f_diffuse;
    varying vec2 f_uv;
    varying float f_gray;
    \n#endif\n

    void main(void)
    {
       gl_Position = convertPosition2(pd_viewRect, pd_vertex, pd_showRect, pd_anRot);
       f_patShowRect = pd_patShowRect;
       f_patAnRot = pd_patAnRot;
       f_diffuse = pd_diffuse;
       float uvX = (pd_vertex.x * pd_texEffect.x + 1.0) * pd_uvRect.z / 2.0 + pd_uvRect.x;
       float uvY = (pd_vertex.y * pd_texEffect.y + 1.0) * pd_uvRect.w / 2.0 + pd_uvRect.y;
       f_uv = vec2(uvX, 1.0 -uvY);
       f_gray = pd_texEffect.z;
    });

    // fragment shader
    const char* gPosColorUVFragShader2D = STRINGIFY(
    \n#ifdef PD_LEVEL_GL3\n
    in vec4 f_patShowRect;
    in vec3 f_patAnRot;
    in vec4 f_diffuse;
    in vec2 f_uv;
    in float f_gray;
    uniform sampler2D pd_texture0;  /* Texture must named "pd_texturex".*/
    out vec4 out_FragColor;
    \n#else\n
    varying vec4 f_patShowRect;
    varying vec3 f_patAnRot;
    varying vec4 f_diffuse;
    varying vec2 f_uv;
    varying float f_gray;
    uniform sampler2D pd_texture0;  /* Texture must named "pd_texturex".*/
    \n#endif\n

    void main(void)
    {
        clipFragment(f_patShowRect, f_patAnRot);
        \n#ifdef PD_LEVEL_GL3\n
        vec4 tecColor = texture(pd_texture0, f_uv) * f_diffuse;
        if (f_gray > 0.0001) {
            float grey = dot(tecColor.rgb, vec3(0.299, 0.587, 0.114));
            out_FragColor = vec4(grey, grey, grey, tecColor.a);
        }
        else {
            out_FragColor = tecColor;
        }
        \n#else\n
        vec4 tecColor = texture2D(pd_texture0, f_uv) * f_diffuse;
        if (f_gray > 0.0001) {
            float grey = tecColor.r*0.299 + tecColor.g*0.587 + tecColor.b*0.114;
            gl_FragColor = vec4(grey, grey, grey, tecColor.a);
        }
        else {
            gl_FragColor = tecColor;
        }
        \n#endif\n
    });

    std::vector<ProgramUniform> gPosColorUVUniforms2D = {ProgramUniform("pd_showRect", UniformDataType::eVector4),
        ProgramUniform("pd_anRot", UniformDataType::eVector3),
        ProgramUniform("pd_patShowRect", UniformDataType::eVector4),
        ProgramUniform("pd_patAnRot", UniformDataType::eVector3),
        ProgramUniform("pd_diffuse", UniformDataType::eVector4),
        ProgramUniform("pd_uvRect", UniformDataType::eVector4),
        ProgramUniform("pd_texEffect", UniformDataType::eVector3)};
}

#endif // PD_POSCOLORUV_SHADER_2D_H
