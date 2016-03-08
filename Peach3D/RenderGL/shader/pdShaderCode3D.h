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
    const char* gVerShaderCode3D = STRINGIFY(\
    #ifdef PD_ENABLE_LIGHT
        \n#define PD_LIGHT_DIRECTION    1.5
        \n#define PD_LIGHT_DOT          2.5
        \n#define PD_LIGHT_SPOT         3.5
    \n#endif
    \n#ifdef PD_LEVEL_GL3
        \n#ifdef PD_ENABLE_LIGHT
            \nuniform LightsUnifroms {
                float pd_lType[PD_LIGHT_COUNT];
                vec3 pd_lPosition[PD_LIGHT_COUNT];
                vec3 pd_lDirection[PD_LIGHT_COUNT];
                vec3 pd_lAttenuate[PD_LIGHT_COUNT];
                vec2 pd_lSpotExtend[PD_LIGHT_COUNT];
                vec3 pd_lAmbient[PD_LIGHT_COUNT];
                vec3 pd_lColor[PD_LIGHT_COUNT];
                vec3 pd_eyeDir;
            };
        \n#endif\n
        uniform GlobalUnifroms {
            mat4 pd_projMatrix;
            mat4 pd_viewMatrix;
        };
        in vec3 pd_vertex;
        \n#ifdef PD_ENABLE_TEXUV\n
            in vec2 pd_uv;
        \n#endif\n
        in mat4 pd_modelMatrix;
        in vec4 pd_diffuse;
        \n#ifdef PD_ENABLE_LIGHT\n
            in vec3 pd_normal;
            in mat4 pd_normalMatrix;
            in vec3 pd_ambient;
            in vec3 pd_specular;
            in float pd_shininess;
            in vec3 pd_emissive;
            out vec3 f_normal;
            out vec3 f_matAmbient;
            out vec3 f_matSpecular;
            out float f_matShininess;
            out vec3 f_matEmissive;
            out vec3 f_lightDir[PD_LIGHT_COUNT];
            out vec3 f_halfVec[PD_LIGHT_COUNT];
            out float f_attenuate[PD_LIGHT_COUNT];
            out vec3 f_lAmbient[PD_LIGHT_COUNT];
            out vec3 f_lColor[PD_LIGHT_COUNT];
        \n#endif\n
        out vec4 f_diffuse;
        \n#ifdef PD_ENABLE_TEXUV\n
            out vec2 f_uv;
        \n#endif
    \n#else\n
        attribute vec3 pd_vertex;
        \n#ifdef PD_ENABLE_TEXUV\n
            attribute vec2 pd_uv;
        \n#endif\n
        uniform mat4 pd_projMatrix;
        uniform mat4 pd_viewMatrix;
        uniform mat4 pd_modelMatrix;
        uniform vec4 pd_diffuse;
        \n#ifdef PD_ENABLE_LIGHT\n
            uniform float pd_lType[PD_LIGHT_COUNT];
            uniform vec3 pd_lPosition[PD_LIGHT_COUNT];
            uniform vec3 pd_lDirection[PD_LIGHT_COUNT];
            uniform vec3 pd_lAttenuate[PD_LIGHT_COUNT];
            uniform vec2 pd_lSpotExtend[PD_LIGHT_COUNT];
            uniform vec3 pd_lAmbient[PD_LIGHT_COUNT];
            uniform vec3 pd_lColor[PD_LIGHT_COUNT];
            uniform vec3 pd_eyeDir;
            attribute vec3 pd_normal;
            uniform mat4 pd_normalMatrix;
            uniform vec3 pd_ambient;
            uniform vec3 pd_specular;
            uniform float pd_shininess;
            uniform vec3 pd_emissive;
            varying vec3 f_normal;
            varying vec3 f_matAmbient;
            varying vec3 f_matSpecular;
            varying float f_matShininess;
            varying vec3 f_matEmissive;
            varying vec3 f_lightDir[PD_LIGHT_COUNT];
            varying vec3 f_halfVec[PD_LIGHT_COUNT];
            varying float f_attenuate[PD_LIGHT_COUNT];
            varying vec3 f_lAmbient[PD_LIGHT_COUNT];
            varying vec3 f_lColor[PD_LIGHT_COUNT];
        \n#endif\n
        varying vec4 f_diffuse;
        \n#ifdef PD_ENABLE_TEXUV\n
            varying vec2 f_uv;
        \n#endif
    \n#endif\n

    void main(void)
    {
        mat4 mvpMatrix = pd_projMatrix * pd_viewMatrix * pd_modelMatrix;
        gl_Position = mvpMatrix * vec4(pd_vertex, 1.0);
        f_diffuse = pd_diffuse;
        \n#ifdef PD_ENABLE_TEXUV\n
            f_uv = pd_uv;
        \n#endif
        \n#ifdef PD_ENABLE_LIGHT\n
            vec4 tnormal = pd_normalMatrix * vec4(pd_normal, 1.0);
            f_normal = normalize(tnormal.xyz);\n  /* Convert normal to world space. */
            for (int i = 0; i < PD_LIGHT_COUNT; ++i) {\n
                float lightType = pd_lType[i];\n                 /* Out light type. */
                f_lAmbient[i] = pd_lAmbient[i];\n
                f_lColor[i] = pd_lColor[i];\n

                /* Out attenuate for dot and spot light. */
                if (lightType > PD_LIGHT_DIRECTION) {\n
                    f_lightDir[i] = pd_lPosition[i] - pd_vertex;\n
                    float lightDis = length(f_lightDir[i]);  \n       /* Light to vertex distance. */
                    f_lightDir[i] = f_lightDir[i] / lightDis;  \n     /* Normalize light direction. */

                    f_attenuate[i] = 1.0 / (pd_lAttenuate[i].x + pd_lAttenuate[i].y * lightDis + pd_lAttenuate[i].z * lightDis * lightDis);\n
                    f_halfVec[i] = normalize(f_lightDir[i] + pd_eyeDir);\n

                    if (lightType > PD_LIGHT_DOT) {\n
                        float spotCos = dot(f_lightDir[i], -pd_lDirection[i]);\n
                        if (spotCos < pd_lSpotExtend[i].x)\n
                            f_attenuate[i] = 0.0;\n
                        else
                            f_attenuate[i]*= pow(spotCos, pd_lSpotExtend[i].y);\n
                    }\n
                }\n
                else {\n
                    f_attenuate[i] = 1.0;\n
                    f_lightDir[i] = -pd_lDirection[i];\n
                    f_halfVec[i] = normalize(f_lightDir[i] + pd_eyeDir);\n
                }
            }
            f_matAmbient = pd_ambient;
            f_matSpecular = pd_specular;
            f_matShininess = pd_shininess;
            f_matEmissive = pd_emissive;
        \n#endif\n
    });

    const char* gFragShaderCode3D = STRINGIFY(\
    #ifdef PD_LEVEL_GL3\n
        in vec4 f_diffuse;
        \n#ifdef PD_ENABLE_TEXUV\n
            in vec2 f_uv;
            uniform sampler2D pd_texture0;
        \n#endif
        \n#ifdef PD_ENABLE_LIGHT\n
            in vec3 f_normal;
            in vec3 f_matAmbient;
            in vec3 f_matSpecular;
            in float f_matShininess;
            in vec3 f_matEmissive;
            in vec3 f_lightDir[PD_LIGHT_COUNT];
            in vec3 f_halfVec[PD_LIGHT_COUNT];
            in float f_attenuate[PD_LIGHT_COUNT];
            in vec3 f_lAmbient[PD_LIGHT_COUNT];
            in vec3 f_lColor[PD_LIGHT_COUNT];
        \n#endif\n
        out vec4 out_FragColor;
    \n#else\n
        varying vec4 f_diffuse;
        \n#ifdef PD_ENABLE_TEXUV\n
            varying vec2 f_uv;
            uniform sampler2D pd_texture0;
        \n#endif
        \n#ifdef PD_ENABLE_LIGHT\n
            varying vec3 f_normal;
            varying vec3 f_matAmbient;
            varying vec3 f_matSpecular;
            varying float f_matShininess;
            varying vec3 f_matEmissive;
            varying vec3 f_lightDir[PD_LIGHT_COUNT];
            varying vec3 f_halfVec[PD_LIGHT_COUNT];
            varying float f_attenuate[PD_LIGHT_COUNT];
            varying vec3 f_lAmbient[PD_LIGHT_COUNT];
            varying vec3 f_lColor[PD_LIGHT_COUNT];
        \n#endif
    \n#endif\n

    void main(void)
    {
        \n#ifdef PD_ENABLE_TEXUV
            \n#ifdef PD_LEVEL_GL3\n
                vec4 tex_color0 = texture( pd_texture0, f_uv );
            \n#else\n
                vec4 tex_color0 = texture2D( pd_texture0, f_uv );
            \n#endif\n
            vec4 fragColor = tex_color0;
        \n#else\n
            vec4 fragColor = vec4(vec3(1.0), f_diffuse.a);
        \n#endif
        /* Calc lighting effect. */
        \n#ifdef PD_ENABLE_LIGHT\n
            vec3 scatteredLight = vec3(0.0);\n
            vec3 reflectedLight = vec3(0.0);\n
            for (int i = 0; i < PD_LIGHT_COUNT; ++i) {\n
                float diffuse = max(0.0, dot(f_normal, f_lightDir[i]));\n
                float specular = max(0.0, dot(f_normal, f_halfVec[i]));\n
                if (diffuse == 0.0)
                    specular = 0.0;
                else
                    specular = pow(specular, f_matShininess);
                scatteredLight += f_lAmbient[i] * f_matAmbient * f_attenuate[i] + f_lColor[i] * f_diffuse.rgb * diffuse * f_attenuate[i];\n
                reflectedLight += f_lColor[i] * f_matSpecular * specular * f_attenuate[i];\n
            }\n
            vec3 rgb = min(f_matEmissive + fragColor.rgb * scatteredLight + reflectedLight, vec3(1.0));\n
            fragColor = vec4(rgb, fragColor.a);\n
        \n#else
            \n#ifdef PD_ENABLE_TEXUV\n
                fragColor = fragColor * f_diffuse;
            \n#else\n
                fragColor = f_diffuse;
            \n#endif
        \n#endif
        \n#ifdef PD_LEVEL_GL3\n
            out_FragColor = fragColor;
        \n#else\n
            gl_FragColor = fragColor;
        \n#endif\n
    });
}

#endif // PD_POSCOLORUV_SHADER_3D_H
