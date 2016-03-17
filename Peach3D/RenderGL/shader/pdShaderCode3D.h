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
    const char* gCommonLightStruct3D = STRINGIFY(\
        struct FragLight
        {
            vec3 lightDir;
            vec3 halfVec;
            float attenuate;
        };\n
        );

    const char* gCommonLighting3D = STRINGIFY(\
        #define PD_LIGHT_DIRECTION    1.5
        \n#define PD_LIGHT_DOT          2.5
        \n#define PD_LIGHT_SPOT         3.5
        \nFragLight calcFragmentLight(float lType, vec3 lPosition, vec3 lDirection, vec3 lAttenuate, vec2 lSpotExtend, vec3 worldVertex, vec3 eyeDir)
        {\n
            FragLight light;
            /* Out attenuate for dot and spot light. */
            if (lType > PD_LIGHT_DIRECTION) {
                light.lightDir = lPosition - worldVertex;
                float lightDis = length(light.lightDir);    /* Light to vertex distance. */
                light.lightDir = light.lightDir / lightDis; /* Normalize light direction. */

                /* Diffuse attenuate and specular halfVec. */
                light.attenuate = 1.0 / (lAttenuate.x + lAttenuate.y * lightDis + lAttenuate.z * lightDis * lightDis);
                light.halfVec = normalize(light.lightDir - eyeDir);

                /* Spot light special attenuate. */
                if (lType > PD_LIGHT_DOT) {
                    float spotCos = dot(light.lightDir, -lDirection);
                    if (spotCos < lSpotExtend.x)
                        light.attenuate = 0.0;
                    else
                        light.attenuate *= pow(spotCos, lSpotExtend.y);
                }
            }
            else {
                /* Direction light just return value. */
                light.attenuate = 1.0;
                light.lightDir = -lDirection;
                light.halfVec = normalize(light.lightDir - eyeDir);
            }
            return light;
        \n}\n
        );

    const char* gVerShaderCode3D = STRINGIFY(\
    #ifdef PD_LEVEL_GL3
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
            out FragLight f_lFrag[PD_LIGHT_COUNT];
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
            attribute vec3 pd_normal;
            uniform mat4 pd_normalMatrix;
            varying vec3 f_normal;
            varying vec3 f_worldVertex;
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
            /* Convert normal to world space. */
            vec4 tnormal = pd_normalMatrix * vec4(pd_normal, 1.0);
            f_normal = normalize(tnormal.xyz);
            /* Convert vertex to world space. */
            vec4 convertVertex = pd_modelMatrix * vec4(pd_vertex, 1.0);
            \n#ifdef PD_LEVEL_GL3\n
                for (int i = 0; i < PD_LIGHT_COUNT; ++i) {
                    /* Out light ambient and color. */
                    f_lAmbient[i] = pd_lAmbient[i];
                    f_lColor[i] = pd_lColor[i];
                    f_lFrag[i] = calcFragmentLight(pd_lType[i], pd_lPosition[i], pd_lDirection[i], pd_lAttenuate[i], pd_lSpotExtend[i], convertVertex.xyz, pd_eyeDir);
                }\n
                f_matAmbient = pd_ambient;
                f_matSpecular = pd_specular;
                f_matShininess = pd_shininess;
                f_matEmissive = pd_emissive;
            \n#else\n
                f_worldVertex = convertVertex.xyz;
            \n#endif
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
            in FragLight f_lFrag[PD_LIGHT_COUNT];
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
            uniform float pd_lType[PD_LIGHT_COUNT];
            uniform vec3 pd_lPosition[PD_LIGHT_COUNT];
            uniform vec3 pd_lDirection[PD_LIGHT_COUNT];
            uniform vec3 pd_lAttenuate[PD_LIGHT_COUNT];
            uniform vec2 pd_lSpotExtend[PD_LIGHT_COUNT];
            uniform vec3 pd_lAmbient[PD_LIGHT_COUNT];
            uniform vec3 pd_lColor[PD_LIGHT_COUNT];
            uniform vec3 pd_eyeDir;
            uniform vec3 pd_ambient;
            uniform vec3 pd_specular;
            uniform float pd_shininess;
            uniform vec3 pd_emissive;
            varying vec3 f_normal;
            varying vec3 f_worldVertex;
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
            vec3 scatteredLight = vec3(0.0);
            vec3 reflectedLight = vec3(0.0);\n
            for (int i = 0; i < PD_LIGHT_COUNT; ++i) {
                \n#ifdef PD_LEVEL_GL3\n
                    float diffuse = max(0.0, dot(f_normal, f_lFrag[i].lightDir));
                    float specular = max(0.0, dot(f_normal, f_lFrag[i].halfVec));
                    if (diffuse == 0.0)
                        specular = 0.0;
                    else
                        specular = pow(specular, f_matShininess);
                    scatteredLight += f_lAmbient[i] * f_matAmbient * f_lFrag[i].attenuate + f_lColor[i] * f_diffuse.rgb * diffuse * f_lFrag[i].attenuate;
                    reflectedLight += f_lColor[i] * f_matSpecular * specular * f_lFrag[i].attenuate;
                \n#else\n
                    FragLight lFrag = calcFragmentLight(pd_lType[i], pd_lPosition[i], pd_lDirection[i], pd_lAttenuate[i], pd_lSpotExtend[i], f_worldVertex, pd_eyeDir);
                    float diffuse = max(0.0, dot(f_normal, lFrag.lightDir));
                    float specular = max(0.0, dot(f_normal, lFrag.halfVec));
                    if (diffuse == 0.0)
                        specular = 0.0;
                    else
                        specular = pow(specular, pd_shininess);
                    scatteredLight += pd_lAmbient[i] * pd_ambient * lFrag.attenuate + pd_lColor[i] * f_diffuse.rgb * diffuse * lFrag.attenuate;
                    reflectedLight += pd_lColor[i] * pd_specular * specular * lFrag.attenuate;
                \n#endif\n
            }
            \n#ifdef PD_LEVEL_GL3\n
                vec3 rgb = min(f_matEmissive + fragColor.rgb * scatteredLight + reflectedLight, vec3(1.0));
            \n#else\n
                vec3 rgb = min(pd_emissive + fragColor.rgb * scatteredLight + reflectedLight, vec3(1.0));
            \n#endif\n
            fragColor = vec4(rgb, fragColor.a);
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
