//
//  pdShaderCode2D.h
//  Peach3DLib
//
//  Created by singoon.he on 10/28/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#ifndef PD_SHADERCODE_2D_H
#define PD_SHADERCODE_2D_H

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

    // vertex shader
    const char* gVerShaderCode2D = STRINGIFY(\
    #ifdef PD_LEVEL_GL3\n
        uniform GlobalUniforms {
            vec4 pd_viewRect;
        };
        in vec2 pd_vertex;
        in vec4 pd_showRect;    /* Widget position under anchor and size.*/
        in vec3 pd_anRot;       /* Widget anchor/rotate.*/
        in vec4 pd_patShowRect; /* Widget parent position under anchor and size.*/
        in vec3 pd_patAnRot;    /* Widget parent anchor/rotate.*/
        in vec4 pd_diffuse;
        \n#ifdef PD_ENABLE_TEXUV\n
            in vec4 pd_uvRect;
            in vec3 pd_texEffect;   /* Sprite scale negative and gray effect. */
        \n#endif\n
        out vec4 f_patShowRect;
        out vec3 f_patAnRot;
        out vec4 f_diffuse;
        \n#ifdef PD_ENABLE_TEXUV\n
            out vec2 f_uv;
            out float f_gray;
        \n#endif
    \n#else\n
        attribute vec2 pd_vertex;
        uniform vec4 pd_viewRect;
        uniform vec4 pd_showRect;   /* Widget position under anchor and size.*/
        uniform vec3 pd_anRot;      /* Widget anchor/rotate.*/
        uniform vec4 pd_patShowRect;/* Widget parent position under anchor and size.*/
        uniform vec3 pd_patAnRot;   /* Widget parent anchor/rotate.*/
        uniform vec4 pd_diffuse;
        \n#ifdef PD_ENABLE_TEXUV\n
            uniform vec4 pd_uvRect;
            uniform vec3 pd_texEffect;  /* Sprite scale negative and gray effect. */
        \n#endif\n
        varying vec4 f_patShowRect;
        varying vec3 f_patAnRot;
        varying vec4 f_diffuse;
        \n#ifdef PD_ENABLE_TEXUV\n
            varying vec2 f_uv;
            varying float f_gray;
        \n#endif
    \n#endif\n

    void main(void)
    {\n
        gl_Position = convertPosition2(pd_viewRect, pd_vertex, pd_showRect, pd_anRot);
        f_patShowRect = pd_patShowRect;
        f_patAnRot = pd_patAnRot;
        f_diffuse = pd_diffuse;
        \n#ifdef PD_ENABLE_TEXUV\n
            float uvX = (pd_vertex.x * pd_texEffect.x + 1.0) * pd_uvRect.z / 2.0 + pd_uvRect.x;
            float uvY = (pd_vertex.y * pd_texEffect.y + 1.0) * pd_uvRect.w / 2.0 + pd_uvRect.y;
            f_uv = vec2(uvX, uvY);
            f_gray = pd_texEffect.z;
        \n#endif\n
    });

    // fragment shader
    const char* gFragShaderCode2D = STRINGIFY(\
    #ifdef PD_LEVEL_GL3\n
        in vec4 f_patShowRect;
        in vec3 f_patAnRot;
        in vec4 f_diffuse;
        \n#ifdef PD_ENABLE_TEXUV\n
            in vec2 f_uv;
            in float f_gray;
            uniform sampler2D pd_texture0;  /* Texture must named "pd_texturex".*/
        \n#endif\n
        out vec4 out_FragColor;
    \n#else\n
        varying vec4 f_patShowRect;
        varying vec3 f_patAnRot;
        varying vec4 f_diffuse;
        \n#ifdef PD_ENABLE_TEXUV\n
            varying vec2 f_uv;
            varying float f_gray;
            uniform sampler2D pd_texture0;  /* Texture must named "pd_texturex".*/
        \n#endif
    \n#endif\n

    void main(void)
    {\n
        clipFragment(f_patShowRect, f_patAnRot);
        \n#ifdef PD_LEVEL_GL3
            \n#ifdef PD_ENABLE_TEXUV\n
                vec4 tecColor = texture(pd_texture0, f_uv) * f_diffuse;
                if (f_gray > 0.0001) {
                    float grey = dot(tecColor.rgb, vec3(0.299, 0.587, 0.114));
                    out_FragColor = vec4(grey, grey, grey, tecColor.a);
                }
                else {
                    out_FragColor = tecColor;
                }
            \n#else\n
                out_FragColor = f_diffuse;
            \n#endif
        \n#else
            \n#ifdef PD_ENABLE_TEXUV\n
                vec4 tecColor = texture2D(pd_texture0, f_uv) * f_diffuse;
                if (f_gray > 0.0001) {
                    float grey = tecColor.r*0.299 + tecColor.g*0.587 + tecColor.b*0.114;
                    gl_FragColor = vec4(grey, grey, grey, tecColor.a);
                }
                else {
                    gl_FragColor = tecColor;
                }
            \n#else\n
                gl_FragColor = f_diffuse;
            \n#endif
        \n#endif\n
    });


    // 2D particle vertex shader
    const char* gVerParticleShaderCode2D = STRINGIFY(\
    #ifdef PD_LEVEL_GL3\n
        in vec2 pd_vertex;
        in vec4 pd_color;       /* Particle point color. */
        in vec2 pd_pSprite;     /* Particle point size and rotate. */
        uniform vec4 pd_viewRect;
        out float out_rotate;
        out vec4 out_color;
    \n#else\n
        attribute vec2 pd_vertex;
        attribute vec4 pd_color;       /* Particle point color. */
        attribute vec2 pd_pSprite;     /* Particle point size and rotate. */
        uniform vec4 pd_viewRect;
        varying float out_rotate;
        varying vec4 out_color;
    \n#endif\n

    void main(void)
    {\n
        gl_Position = vec2(pd_vertex.x/pd_viewRect.z - 1.0, pd_vertex.y/pd_viewRect.w - 1.0);
        gl_PointSize = pd_pSprite.x;
        out_rotate = pd_pSprite.y;
        out_color = pd_color;
        \n#endif\n
    });

    // 2D particle fragment shader
    const char* gFragParticleShaderCode2D = STRINGIFY(\
    #ifdef PD_LEVEL_GL3\n
        in float out_rotate;
        in vec4 out_color;
        uniform sampler2D pd_texture0;  /* Texture must named "pd_texturex".*/
        out vec4 out_FragColor;
    \n#else\n
        varying float out_rotate;
        varying vec4 out_color;
        uniform sampler2D pd_texture0;  /* Texture must named "pd_texturex".*/
    \n#endif\n

    void main(void)
    {\n
        vec2 rotatedUV = gl_PointCoord;
        if (out_rotate > 0.0001 || out_rotate < -0.0001) {
            rotatedUV = vec2(cos(out_rotate) * (gl_PointCoord.x - 0.5) + sin(out_rotate) * (gl_PointCoord.y - 0.5) + 0.5,
                        cos(out_rotate) * (gl_PointCoord.y - 0.5) - sin(out_rotate) * (gl_PointCoord.x - 0.5) + 0.5);
        }
        \n#ifdef PD_LEVEL_GL3
            out_FragColor = texture(pd_texture0, rotatedUV) * out_color;
        \n#else\n
            gl_FragColor = texture2D(pd_texture0, rotatedUV) * out_color;
        \n#endif\n
    });
}

#endif // PD_SHADERCODE_2D_H
