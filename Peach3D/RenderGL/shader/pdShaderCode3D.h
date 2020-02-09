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
    /************************************** GL3 shders ***************************************/
    const char* gVerGL3ShaderCode3D = R"(
    #ifdef PD_ENABLE_SKELETON
        uniform BoneUniforms {
            vec4 pd_boneMatrix[PD_ENABLE_SKELETON * 3];
        };
        in vec4 pd_bWidget;
        in vec4 pd_bIndex;
    #endif
    uniform GlobalUniforms {
        mat4 pd_projMatrix;
        mat4 pd_viewMatrix;
    };
    in vec3 pd_vertex;
    in mat4 pd_modelMatrix;
    in vec4 pd_diffuse;
    out vec4 f_diffuse;
    #ifdef PD_ENABLE_TEXUV
        in vec2 pd_uv;
        out vec2 f_uv;
    #endif
    #ifdef PD_ENABLE_LIGHT
        in vec3 pd_normal;
        in mat4 pd_normalMatrix;
        // !vertex attribute must written in vertex shader
        in vec3 pd_ambient;
        in vec4 pd_specular; /* Include material shininess. */
        in vec3 pd_emissive;
        out vec3 f_ambient;
        out vec4 f_specular;
        out vec3 f_emissive;
        out vec3 f_normal;
        out vec4 f_worldPos;
        #ifdef PD_SHADOW_COUNT
            /* Define shadow uniforms when light enabled. */
            uniform ShadowUniforms {
                mat4 pd_shadowMatrix[PD_SHADOW_COUNT];
            };
            out vec4 f_shadowCoord[PD_SHADOW_COUNT];
        #endif
    #endif

    void main(void)
    {
        vec4 attribPos = vec4(pd_vertex, 1.0);
        #ifdef PD_ENABLE_SKELETON
            vec4 vMtX = vec4(0.0);
            vec4 vMtY = vec4(0.0);
            vec4 vMtZ = vec4(0.0);
            bool isUsedBones = false;
            for (int i = 0; i < 4; ++i){
                if (pd_bWidget[i] > 0.0) {
                    int boneIndex = int(pd_bIndex[i]) * 3;
                    vMtX += pd_boneMatrix[boneIndex] * pd_bWidget[i];
                    vMtY += pd_boneMatrix[boneIndex + 1] * pd_bWidget[i];
                    vMtZ += pd_boneMatrix[boneIndex + 2] * pd_bWidget[i];
                    isUsedBones = true;
                }
            }
            if (!isUsedBones) {
                vMtX.x = 1.0;
                vMtY.y = 1.0;
                vMtZ.z = 1.0;
            }
            vec4 resPos = attribPos;
            resPos.x = dot(attribPos, vMtX);
            resPos.y = dot(attribPos, vMtY);
            resPos.z = dot(attribPos, vMtZ);
            attribPos = resPos;
        #endif
        vec4 worldPos = pd_modelMatrix * attribPos;
        mat4 vpMatrix = pd_projMatrix * pd_viewMatrix;
        gl_Position = vpMatrix * worldPos;
        // !vertex attribute must written in vertex shader
        f_diffuse = pd_diffuse;
        #ifdef PD_ENABLE_TEXUV
            f_uv = pd_uv;
        #endif
        #ifdef PD_ENABLE_LIGHT
            f_worldPos = worldPos;
            /* Convert normal to world space. */
            vec4 tnormal = pd_normalMatrix * vec4(pd_normal, 1.0);
            f_normal = normalize(tnormal.xyz);
            // 
            f_ambient = pd_ambient;
            f_specular = pd_specular;
            f_emissive = pd_emissive;
            // out world shadow pos
            #ifdef PD_SHADOW_COUNT
                for (int i = 0; i < PD_SHADOW_COUNT; ++i) {
                    f_shadowCoord[i] = pd_shadowMatrix[i] * worldPos;
                }
            #endif
        #endif
    })";

    const char* gFragGL3ShaderCode3D = R"(
    #ifdef PD_ENABLE_LIGHT
        #define PD_LIGHT_DIRECTION    1.5
        #define PD_LIGHT_DOT          2.5
        #define PD_LIGHT_SPOT         3.5
        /* Use std140 to make unify same offset for vec3, some verder maybe different. */
        layout (std140) uniform LightsUniforms {
            vec4 pd_lTypeSpot[PD_LIGHT_COUNT];  /* Light type and spot light extend attenuate, shadow enabled(1.0) or 0.0. */
            vec3 pd_lPosition[PD_LIGHT_COUNT];  /* Dot light or spot light position. */
            vec3 pd_lDirection[PD_LIGHT_COUNT]; /* Direction light or spot light direction. */
            vec3 pd_lAttenuate[PD_LIGHT_COUNT]; /* Dot light or spot light base attenuate. */
            vec3 pd_lColor[PD_LIGHT_COUNT];     /* Light color. */
            vec3 pd_eyeDir;                     /* Active camera direction. */
            vec3 pd_gAmbient;                   /* Scene global ambient. */
        };
        in vec3 f_ambient;
        in vec4 f_specular; /* Include material shininess. */
        in vec3 f_emissive;
        in vec4 f_worldPos;
        in vec3 f_normal;
        #ifdef PD_SHADOW_COUNT
          in vec4 f_shadowCoord[PD_SHADOW_COUNT];
          uniform sampler2DShadow pd_shadowTexture[PD_SHADOW_COUNT];
        #endif
    #endif
    in vec4 f_diffuse;
    #ifdef PD_ENABLE_TEXUV
        in vec2 f_uv;
        uniform sampler2D pd_texture0;
    #endif
    out vec4 out_FragColor;

    void main(void)
    {
        #ifdef PD_ENABLE_TEXUV
            vec4 fragColor = texture(pd_texture0, f_uv);
        #else
            vec4 fragColor = vec4(vec3(1.0), f_diffuse.a);
        #endif
        /* Calc lighting effect. */
        #ifdef PD_ENABLE_LIGHT
            vec3 scatteredLight = vec3(0.0);
            vec3 reflectedLight = vec3(0.0);
            #ifdef PD_SHADOW_COUNT
                int shadowIndex = 0;
            #endif
            for (int i = 0; i < PD_LIGHT_COUNT; ++i) {
                float lType = pd_lTypeSpot[i].x;
                vec3 vertexDir;
                vec4 halfAtten;
                if (lType > PD_LIGHT_DIRECTION) {
                    vertexDir = pd_lPosition[i] - f_worldPos.xyz;
                    float lightDis = length(vertexDir); /* Light to vertex distance. */
                    vertexDir = vertexDir / lightDis; /* Normalize light direction. */

                    /* Calc attenuate and specular halfVec. */
                    float verAtten = 1.0 / (pd_lAttenuate[i].x + pd_lAttenuate[i].y * lightDis + pd_lAttenuate[i].z * lightDis * lightDis);
                    if (lType > PD_LIGHT_DOT) {
                        float spotCos = dot(vertexDir, -pd_lDirection[i]);
                        if (spotCos < pd_lTypeSpot[i].y)
                            verAtten = 0.0;
                        else
                            verAtten *= pow(spotCos, pd_lTypeSpot[i].z);
                    }
                    halfAtten = vec4(normalize(vertexDir - pd_eyeDir), verAtten);
                }
                else {
                    /* Direction light just return value. */
                    vertexDir = -pd_lDirection[i];
                    halfAtten = vec4(normalize(vertexDir - pd_eyeDir), 1.0);
                }

                float diffuse = max(0.0, dot(f_normal, vertexDir));
                float specular = max(0.0, dot(f_normal, halfAtten.xyz));
                if (diffuse < 0.0001)
                    specular = 0.0;
                else
                    specular = pow(specular, f_specular.a);

                float shadowAtten = 1.0;    // shadow attenuation
                #ifdef PD_SHADOW_COUNT
                    if (pd_lTypeSpot[i].w > 0.5) {
                        // Dynamic indexing of sampler types is not allowed on GLES3
                        switch (shadowIndex) {
                            case 0:
                                shadowAtten = textureProj(pd_shadowTexture[0], f_shadowCoord[shadowIndex]);
                                break;
                            #if PD_SHADOW_COUNT > 1
                            case 1:
                                shadowAtten = textureProj(pd_shadowTexture[1], f_shadowCoord[shadowIndex]);
                                break;
                            #if PD_SHADOW_COUNT > 2
                            case 2:
                                shadowAtten = textureProj(pd_shadowTexture[2], f_shadowCoord[shadowIndex]);
                                break;
                            #if PD_SHADOW_COUNT > 3
                            case 3:
                                shadowAtten = textureProj(pd_shadowTexture[3], f_shadowCoord[shadowIndex]);
                                break;
                            #endif
                            #endif
                            #endif
                            default:
                                break;
                        }
                        shadowIndex = shadowIndex + 1;
                    }
                #endif
                // accumulate diffuse and specular color
                float verAtten = halfAtten.w;
                scatteredLight += pd_lColor[i] * f_diffuse.rgb * diffuse * verAtten * shadowAtten;
                reflectedLight += pd_lColor[i] * f_specular.rgb * specular * verAtten * shadowAtten;
            }
            vec3 rgb = min(f_emissive + fragColor.rgb * (scatteredLight + pd_gAmbient * f_ambient) + reflectedLight, vec3(1.0));
            fragColor = vec4(rgb, fragColor.a);
        #else
            #ifdef PD_ENABLE_TEXUV
                fragColor = fragColor * f_diffuse;
            #else
                fragColor = f_diffuse;
            #endif
        #endif
        out_FragColor = fragColor;
    })";

    /************************************** GL2 shders ***************************************/

    const char* gVerGL2ShaderCode3D = STRINGIFY(\
    attribute vec3 pd_vertex;
    \n#ifdef PD_ENABLE_TEXUV\n
        attribute vec2 pd_uv;
    \n#endif
    \n#ifdef PD_ENABLE_SKELETON\n
        attribute vec4 pd_bWidget;
        attribute vec4 pd_bIndex;
        uniform vec4 pd_boneMatrix[PD_ENABLE_SKELETON * 3];
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
        \n#ifdef PD_SHADOW_COUNT\n
            /* Define shadow uniforms when light enabled. */
            uniform mat4 pd_shadowMatrix[PD_SHADOW_COUNT];
            varying vec4 f_shadowCoord[PD_SHADOW_COUNT];
        \n#endif
    \n#endif\n
    varying vec4 f_diffuse;
    \n#ifdef PD_ENABLE_TEXUV\n
        varying vec2 f_uv;
    \n#endif\n

    void main(void)
    {
        vec4 attribPos = vec4(pd_vertex, 1.0);
        \n#ifdef PD_ENABLE_SKELETON\n
            vec4 vMtX = vec4(0.0);
            vec4 vMtY = vec4(0.0);
            vec4 vMtZ = vec4(0.0);
            bool isUsedBones = false;
            for (int i = 0; i < 4; ++i){
                if (pd_bWidget[i] > 0.0) {
                    int boneIndex = int(pd_bIndex[i]) * 3;
                    vMtX += pd_boneMatrix[boneIndex] * pd_bWidget[i];
                    vMtY += pd_boneMatrix[boneIndex + 1] * pd_bWidget[i];
                    vMtZ += pd_boneMatrix[boneIndex + 2] * pd_bWidget[i];
                    isUsedBones = true;
                }
            }
            if (!isUsedBones) {
                vMtX.x = 1.0;
                vMtY.y = 1.0;
                vMtZ.z = 1.0;
            }
            vec4 resPos = attribPos;
            resPos.x = dot(attribPos, vMtX);
            resPos.y = dot(attribPos, vMtY);
            resPos.z = dot(attribPos, vMtZ);
            attribPos = resPos;
        \n#endif\n
        vec4 worldPos = pd_modelMatrix * attribPos;
        mat4 vpMatrix = pd_projMatrix * pd_viewMatrix;
        gl_Position = vpMatrix * worldPos;
        f_diffuse = pd_diffuse;
        \n#ifdef PD_ENABLE_TEXUV\n
            f_uv = pd_uv;
        \n#endif
        \n#ifdef PD_ENABLE_LIGHT\n
            /* Convert normal to world space. */
            vec4 tnormal = pd_normalMatrix * vec4(pd_normal, 1.0);
            f_normal = normalize(tnormal.xyz);
            f_worldVertex = worldPos.xyz;
            \n#ifdef PD_SHADOW_COUNT\n
                for (int i = 0; i < PD_SHADOW_COUNT; ++i) {
                    f_shadowCoord[i] = pd_shadowMatrix[i] * worldPos;
                }
            \n#endif
        \n#endif\n
    });

    const char* gFragGL2ShaderCode3D = STRINGIFY(\
    #ifdef PD_ENABLE_LIGHT
        \n#define PD_LIGHT_DIRECTION    1.5
        \n#define PD_LIGHT_DOT          2.5
        \n#define PD_LIGHT_SPOT         3.5
    \n#endif\n
    varying vec4 f_diffuse;
    \n#ifdef PD_ENABLE_TEXUV\n
        varying vec2 f_uv;
        \n#ifdef PD_ENABLE_TERRAIN\n
            uniform sampler2D pd_texture[PD_ENABLE_TERRAIN];
            uniform sampler2D pd_alphaMap0;
            \n#if PD_ENABLE_TERRAIN > 4\n
                uniform sampler2D pd_alphaMap1;
            \n#endif\n
            uniform float pd_detailSize[PD_ENABLE_TERRAIN];
        \n#else\n
            uniform sampler2D pd_texture0;
        \n#endif
    \n#endif
    \n#ifdef PD_ENABLE_LIGHT\n
        uniform vec4 pd_lTypeSpot[PD_LIGHT_COUNT];  /* Light type and spot light extend attenuate, shadow enabled(1.0) or 0.0. */
        uniform vec3 pd_lPosition[PD_LIGHT_COUNT];  /* Dot light or spot light position. */
        uniform vec3 pd_lDirection[PD_LIGHT_COUNT]; /* Direction light or spot light direction. */
        uniform vec3 pd_lAttenuate[PD_LIGHT_COUNT]; /* Dot light or spot light base attenuate. */
        uniform vec3 pd_lColor[PD_LIGHT_COUNT];     /* Light color. */
        uniform vec3 pd_eyeDir;
        uniform vec3 pd_ambient;
        uniform vec4 pd_specular; /* Include material shininess. */
        uniform vec3 pd_emissive;
        uniform vec3 pd_gAmbient;
        varying vec3 f_normal;
        varying vec3 f_worldVertex;
        \n#ifdef PD_SHADOW_COUNT\n
            varying vec4 f_shadowCoord[PD_SHADOW_COUNT];
            uniform sampler2D pd_shadowTexture[PD_SHADOW_COUNT];
        \n#endif
    \n#endif\n

    void main(void)
    {
        \n#ifdef PD_ENABLE_TEXUV\n
            \n#if defined(PD_ENABLE_TERRAIN)\n
                vec4 blendFactor =texture2D(pd_alphaMap0, f_uv);
                vec4 fragColor = texture2D(pd_texture[0], f_uv * pd_detailSize[0]) * blendFactor.r;
                \n#if (PD_ENABLE_TERRAIN > 1) \n
                    fragColor = fragColor + texture2D(pd_texture[1], f_uv * pd_detailSize[1]) * blendFactor.g;
                    \n#if (PD_ENABLE_TERRAIN > 2) \n
                        fragColor = fragColor + texture2D(pd_texture[2], f_uv * pd_detailSize[2]) * blendFactor.b;
                        \n#if (PD_ENABLE_TERRAIN > 3) \n
                            fragColor = fragColor + texture2D(pd_texture[3], f_uv * pd_detailSize[3]) * (1.0 - blendFactor.a);
                            /* sampling second alpha texture. */
                            \n#if (PD_ENABLE_TERRAIN > 4) \n
                                vec4 blendFactor1 =texture2D(pd_alphaMap1, f_uv);
                                fragColor = fragColor + texture2D(pd_texture[4], f_uv * pd_detailSize[4]) * blendFactor1.r;
                                \n#if (PD_ENABLE_TERRAIN > 5) \n
                                    fragColor = fragColor + texture2D(pd_texture[5], f_uv * pd_detailSize[5]) * blendFactor1.g;
                                    \n#if (PD_ENABLE_TERRAIN > 6) \n
                                        fragColor = fragColor + texture2D(pd_texture[6], f_uv * pd_detailSize[6]) * blendFactor1.b;
                                        \n#if (PD_ENABLE_TERRAIN > 7) \n
                                            fragColor = fragColor + texture2D(pd_texture[7], f_uv * pd_detailSize[7]) * (1.0 - blendFactor1.a);
                                        \n#endif\n
                                    \n#endif\n
                                \n#endif\n
                            \n#endif\n
                        \n#endif\n
                    \n#endif\n
                \n#endif\n
                /* Terrain not need alpha. */
                fragColor.a = 1.0;
            \n#else\n
                vec4 fragColor = texture2D( pd_texture0, f_uv );
            \n#endif\n
        \n#else\n
            vec4 fragColor = vec4(vec3(1.0), f_diffuse.a);
        \n#endif
        /* Calc lighting effect. */
        \n#ifdef PD_ENABLE_LIGHT\n
            vec3 scatteredLight = vec3(0.0);
            vec3 reflectedLight = vec3(0.0);
            vec3 lightDir = vec3(0.0);
            vec3 halfVec = vec3(0.0);
            \n#ifdef PD_SHADOW_COUNT\n
                int shadowIndex = 0;
            \n#endif\n
            for (int i = 0; i < PD_LIGHT_COUNT; ++i) {
                float curAttenuate = 1.0;
                float lType = pd_lTypeSpot[i].x;
                if (lType > PD_LIGHT_DIRECTION) {
                    lightDir = pd_lPosition[i] - f_worldVertex;
                    float lightDis = length(lightDir); /* Light to vertex distance. */
                    lightDir = lightDir / lightDis; /* Normalize light direction. */

                    /* Calc attenuate and specular halfVec. */
                    curAttenuate = 1.0 / (pd_lAttenuate[i].x + pd_lAttenuate[i].y * lightDis + pd_lAttenuate[i].z * lightDis * lightDis);
                    if (lType > PD_LIGHT_DOT) {
                        float spotCos = dot(lightDir, -pd_lDirection[i]);
                        if (spotCos < pd_lTypeSpot[i].y)
                            curAttenuate = 0.0;
                        else
                            curAttenuate *= pow(spotCos, pd_lTypeSpot[i].z);
                    }
                }
                else {
                    lightDir = -pd_lDirection[i];
                }
                halfVec = normalize(lightDir - pd_eyeDir);
                // accumulate diffuse and specular color
                float diffuse = max(0.0, dot(f_normal, lightDir));
                float specular = max(0.0, dot(f_normal, halfVec));
                if (diffuse == 0.0)
                    specular = 0.0;
                else
                    specular = pow(specular, pd_specular.a);

                float shadowAtten = 1.0;    // shadow attenuation
                \n#ifdef PD_SHADOW_COUNT\n
                    if (pd_lTypeSpot[i].w > 0.5) {
                        // convert shadow pos to (0-1), can't move to vertex shader (lerp will made error)
                        vec4 shadowMapPosition = f_shadowCoord[shadowIndex] / f_shadowCoord[shadowIndex].w;
                        float distanceFromLight = texture2D(pd_shadowTexture[shadowIndex], shadowMapPosition.st).z;
                        shadowAtten = float(distanceFromLight > shadowMapPosition.z);
                        shadowIndex = shadowIndex + 1;
                    }
                \n#endif\n
                scatteredLight += pd_lColor[i] * f_diffuse.rgb * diffuse * curAttenuate * shadowAtten;
                reflectedLight += pd_lColor[i] * pd_specular.rgb * specular * curAttenuate * shadowAtten;
            }
            vec3 rgb = min(pd_emissive + fragColor.rgb * (scatteredLight + pd_gAmbient * pd_ambient) + reflectedLight, vec3(1.0));
            fragColor = vec4(rgb, fragColor.a);
        \n#else
            \n#ifdef PD_ENABLE_TEXUV\n
                fragColor = fragColor * f_diffuse;
            \n#else\n
                fragColor = f_diffuse;
            \n#endif
        \n#endif\n
        gl_FragColor = fragColor;\n
    });

    /************************************** particle3d shders ***************************************/
    // 3D particle vertex shader
    const char* gVerParticleShaderCode3D = STRINGIFY(\
    #ifdef PD_LEVEL_GL3\n
        in vec3 pd_vertex;
        in vec4 pd_color;       /* Particle point color. */
        in vec2 pd_pSprite;     /* Particle point size and rotate. */
        uniform mat4 pd_projMatrix;
        uniform mat4 pd_viewMatrix;
        out float out_rotate;
        out vec4 out_color;
    \n#else\n
        attribute vec3 pd_vertex;
        attribute vec4 pd_color;       /* Particle point color. */
        attribute vec2 pd_pSprite;     /* Particle point size and rotate. */
        uniform mat4 pd_projMatrix;
        uniform mat4 pd_viewMatrix;
        varying float out_rotate;
        varying vec4 out_color;
    \n#endif\n

    void main(void)
    {\n
        vec4 eyePos = pd_viewMatrix * vec4(pd_vertex, 1.0);
        gl_Position = pd_projMatrix * eyePos;
        vec4 offV = pd_projMatrix * vec4(pd_pSprite.x, 0, 0, 1);
        gl_PointSize = offV.x;
        out_rotate = pd_pSprite.y;
        out_color = pd_color;
    \n});

    // 3D particle fragment shader
    const char* gFragParticleShaderCode3D = STRINGIFY(\
    #ifdef PD_LEVEL_GL3\n
        in float out_rotate;
        in vec4 out_color;
        out vec4 out_FragColor;
    \n#else\n
        varying float out_rotate;
        varying vec4 out_color;
    \n#endif\n
    uniform sampler2D pd_texture0;  /* Texture must named "pd_texturex".*/
    uniform vec4      pd_uvRect;

    void main(void)
    {\n
        vec2 rotatedUV = gl_PointCoord;
        rotatedUV.x = rotatedUV.x * pd_uvRect.z + pd_uvRect.x;
        rotatedUV.y = (1.0 - rotatedUV.y) * pd_uvRect.w + pd_uvRect.y; /* gl_PointCoord used top left, but texture used top bottom.*/
        if (out_rotate > 0.0001 || out_rotate < -0.0001) {\n
            vec2 center = vec2(pd_uvRect.z * 0.5 + pd_uvRect.x, pd_uvRect.w * 0.5 + pd_uvRect.y);
            float sinR = sin(out_rotate);
            float cosR = cos(out_rotate);
            float aspect = pd_uvRect.w / pd_uvRect.z;
            mat2 rotateMat = mat2(cosR, sinR * aspect, -sinR / aspect, cosR);
            rotatedUV = rotateMat * vec2(rotatedUV.x - center.x, rotatedUV.y - center.y) + center;
        \n}
        \n#ifdef PD_LEVEL_GL3\n
            out_FragColor = texture(pd_texture0, rotatedUV) * out_color;
        \n#else\n
            gl_FragColor = texture2D(pd_texture0, rotatedUV) * out_color;
        \n#endif\n
    });
}

#endif // PD_POSCOLORUV_SHADER_3D_H
