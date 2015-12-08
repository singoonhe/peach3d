//
//  pdCommonFuncShader2D.h
//  Peach3DLib
//
//  Created by singoon.he on 10/28/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#ifndef PD_COMMONFUNC_SHADER_2D_H
#define PD_COMMONFUNC_SHADER_2D_H

#include "Peach3DShaderCode.h"

namespace Peach3D
{
    // vertex shader
    const char* gCommonVertexFunc2D = "vec4 convertPosition2(vec4 viewRect, vec2 vertex, vec4 showRect, vec3 anRot) { \n"
    "   vec4 relRect = vec4(showRect.x/viewRect.z, showRect.y/viewRect.w, showRect.z/viewRect.z, showRect.w/viewRect.w); \n"
    "   vec2 renderPos = vec2(relRect.x - relRect.z * anRot.x, relRect.y - relRect.w * anRot.y); \n"
    "   /* Convert vertexs to widget rect. algorithm : x=w*(v.x+1)+2*p.x-1. */ \n"
    "   float posX = relRect.z * (vertex.x + 1.0) + 2.0 * renderPos.x - 1.0; \n"
    "   float posY = relRect.w * (vertex.y + 1.0) + 2.0 * renderPos.y - 1.0; \n"
    "   if (anRot.z > 0.0001) { \n"
    "       float aspect = viewRect.z / viewRect.w; \n"
    "       /* Rotate with aspect, because current vertex is relative pos.*/ \n"
    "       mat2 rotateMat = mat2(cos(anRot.z), sin(anRot.z) * aspect, -sin(anRot.z) / aspect, cos(anRot.z)); \n"
    "       vec2 offset = vec2(relRect.x * 2.0 - 1.0, relRect.y * 2.0 - 1.0); \n"
    "       /* Move anchor point to zero, then rotate.*/ \n"
    "       vec2 rPos = rotateMat * vec2(posX - offset.x, posY - offset.y); \n"
    "       /* Move rotate point to orign position.*/ \n"
    "       return vec4(rPos.x + offset.x, rPos.y + offset.y, 0.0, 1.0); \n"
    "   } \n"
    "   else { \n"
    "       return vec4(posX, posY, 0.0, 1.0); \n"
    "   } \n"
    "} \n";
    const char* gCommonFragClipFunc2D = "void clipFragment(vec4 patShowRect, vec3 patAnRot) { \n"
    "   vec2 anchorSize = vec2(patAnRot.x * patShowRect.z, patAnRot.y * patShowRect.w); \n"
    "   if (patAnRot.z > 0.0001) { \n"
    "       vec2 offset = vec2(gl_FragCoord.x - patShowRect.x, gl_FragCoord.y - patShowRect.y); \n"
    "       /* Rotate gl_FragCoord nevgative parent rotation.*/ \n"
    "       mat2 rotateMat = mat2(cos(patAnRot.z), -sin(patAnRot.z), sin(patAnRot.z), cos(patAnRot.z)); \n"
    "       offset = rotateMat * offset + anchorSize; \n"
    "       if (offset.x < 0.0 || offset.y < 0.0 || offset.x > patShowRect.z || offset.y > patShowRect.w) discard; \n"
    "   } \n"
    "   else { \n"
    "       vec2 startPos = vec2(patShowRect.x - anchorSize.x, patShowRect.y - anchorSize.y); \n"
    "       if (gl_FragCoord.x < startPos.x || gl_FragCoord.y < startPos.y || gl_FragCoord.x > (startPos.x + patShowRect.z) || gl_FragCoord.y > (startPos.y + patShowRect.w)) discard; \n"
    "   } \n"
    "} \n";
}

#endif // PD3_POSCOLOR_SHADER_2D_H
