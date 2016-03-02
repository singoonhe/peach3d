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
    const char* gPosColorUVVerShader3D = \
    STRINGIFY(
              \n#ifdef PD_ENABLE_LIGHT
              \n#define PD_LIGHT_DIRECTION    1
              \n#define PD_LIGHT_DOT          2
              \n#define PD_LIGHT_SPOT         3
              \nstruct LightVertex
              {
                  int type;
                  vec3 position;
                  vec3 direction;
                  vec3 attenuate;
                  vec2 spotExtend;
              };
              \n#endif\n
              uniform GlobalUnifroms {
                  mat4 pd_projMatrix;
                  mat4 pd_viewMatrix;
                  vec3 pd_eyeDir;
              };\n
              in vec3 pd_vertex;
              \n#ifdef PD_ENABLE_TEXUV\n
              in vec2 pd_uv;
              \n#endif\n
              in mat4 pd_modelMatrix;
              in vec4 pd_diffuse;
              \n#ifdef PD_ENABLE_LIGHT\n
              in vec3 pd_normal;
              in mat4 pd_normalMatrix;
              LightVertex lights[PD_LIGHT_COUNT];
              out vec3 f_normal;
              out vec3 f_lightDir[PD_LIGHT_COUNT];
              out vec3 f_halfVec[PD_LIGHT_COUNT];
              out float f_attenuate[PD_LIGHT_COUNT];
              \n#endif\n
              out vec4 f_diffuse;
              \n#ifdef PD_ENABLE_TEXUV\n
              out vec2 f_uv;
              \n#endif\n
              
              void main(void)
              {
                  mat4 mvpMatrix = pd_projMatrix * pd_viewMatrix * pd_modelMatrix;
                  gl_Position = mvpMatrix * vec4(pd_vertex, 1.0);
                  f_diffuse = pd_diffuse;
                  f_uv = pd_uv;
                  \n#ifdef PD_ENABLE_LIGHT\n
                  f_normal = normalize(pd_normalMatrix * pd_normal);  /* Convert normal to world space. */
                  for (int i = 0; i < PD_LIGHT_COUNT; ++i) {
                      int lightType = lights[i].type;                 /* Out light type. */
                      
                      /* Out attenuate for dot and spot light. */
                      if (lightType!= PD_LIGHT_DIRECTION) {
                          f_lightDir[i] = lights[i].position - pd_vertex;
                          float lightDis = length(f_lightDir[i]);         /* Light to vertex distance. */
                          f_lightDir[i] = f_lightDir[i] / lightDis;       /* Normalize light direction. */
                          
                          f_attenuate[i] = 1.0 / (lights[i].attenuate.x + lights[i].attenuate.y * lightDis + lights[i].attenuate.z * lightDis * lightDis);
                          f_halfVec[i] = normalize(f_lightDir[i] + pd_eyeDir);
                          
                          if (lightType == PD_LIGHT_SPOT) {
                              float spotCos = dot(f_lightDir[i], -lights[i].direction);
                              if (spotCos < lights[i].spotExtend.x)
                                  f_attenuate[i] = 0.0;
                              else
                                  f_attenuate[i]*= pow(spotCos, lights[i].spotExtend.y);
                          }
                      }
                      else {
                          f_attenuate[i] = 1.0;
                          f_lightDir[i] = lights[i].direction;
                          f_halfVec[i] = normalize(lights[i].direction + pd_eyeDir);
                      }
                  }
                  \n#endif\n
              });

    const char* gPosColorUVFragShader3D = STRINGIFY(
        \n#ifdef PD_ENABLE_LIGHT
        \nstruct LightFragment
        {
            vec3 ambient;
            vec3 color;
        };
        \n#endif\n
        in vec4 f_diffuse;
        in vec2 f_uv;
        uniform sampler2D pd_texture0;
        \n#ifdef PD_ENABLE_LIGHT\n
        in vec3 f_normal;
        in vec3 pd_ambient;
        in vec3 pd_specular;
        in float pd_shininess;
        in vec3 pd_emissive;
        in LightFragment lights[PD_LIGHT_COUNT];
        in vec3 f_lightDir[PD_LIGHT_COUNT];
        in vec3 f_halfVec[PD_LIGHT_COUNT];
        in float f_attenuate[PD_LIGHT_COUNT];
        \n#endif\n
        out vec4 out_FragColor;

        void main(void)
        {
            vec4 tex_color0 = texture( pd_texture0, f_uv );
            vec4 fragColor = f_diffuse * tex_color0;
            \n#ifdef PD_ENABLE_LIGHT\n
            vec3 scatteredLight(0.0);
            vec3 reflectedLight(0.0);
            for (int i = 0; i < PD_LIGHT_COUNT; ++i) {
                float diffuse = max(0.0, dot(f_normal, f_lightDir[i]));
                float specular = max(0.0, dot(f_normal, f_halfVec[i]));
                scatteredLight += lights[i].ambient * pd_ambient * f_attenuate[i] + lights[i].color * f_diffuse.rgb * diffuse * f_attenuate[i];
                reflectedLight += lights[i].color * pd_specular * specular * f_attenuate[i];
            }
            vec3 rgb = min(pd_emissive + fragColor.rgb * scatteredLight + reflectedLight, vec3(1.0));
            out_FragColor = vec4(rgb, fragColor.a);
            \n#else\n
            out_FragColor = fragColor;
            \n#endif\n
        });

    /*
    // vertex shader
    const char* gPosColorUVVerShader3D = STRINGIFY(
    \n#ifdef PD_LEVEL_GL3\n
    uniform GlobalUnifroms {
        mat4 pd_projMatrix;
        mat4 pd_viewMatrix;
        \n#ifdef PD_ENABLE_LIGHT\n
        vec3 pd_viewDir;
        \n#endif
    };\n
    in vec3 pd_vertex;
    in vec2 pd_uv;
    \n#ifdef PD_ENABLE_LIGHT\n
    in vec3 pd_normal;
    \n#endif\n
    in mat4 pd_modelMatrix;
    in vec4 pd_diffuse;
    out vec4 f_diffuse;
    out vec2 f_uv;
    \n#else\n
    attribute vec3 pd_vertex;
    attribute vec2 pd_uv;
    uniform mat4 pd_projMatrix;
    uniform mat4 pd_viewMatrix;
    uniform mat4 pd_modelMatrix;
    uniform vec4 pd_diffuse;
    varying vec4 f_diffuse;
    varying vec2 f_uv;
    \n#endif\n

    void main(void)
    {
        mat4 mvpMatrix = pd_projMatrix * pd_viewMatrix * pd_modelMatrix;
        gl_Position = mvpMatrix * vec4(pd_vertex, 1.0);
        f_diffuse = pd_diffuse;
        f_uv = pd_uv;
    }); */

    /*
    // fragment shader
    const char* gPosColorUVFragShader3D = STRINGIFY(
    \n#ifdef PD_LEVEL_GL3\n
    in vec4 f_diffuse;
    in vec2 f_uv;
    uniform sampler2D pd_texture0;
    out vec4 out_FragColor;
    \n#else\n
    varying vec4 f_diffuse;
    varying vec2 f_uv;
    uniform sampler2D pd_texture0;
    \n#endif\n

    void main(void)
    {
        \n#ifdef PD_LEVEL_GL3\n
        vec4 tex_color0 = texture( pd_texture0, f_uv );
        out_FragColor = f_diffuse * tex_color0;
        \n#else\n
        vec4 tex_color0 = texture2D( pd_texture0, f_uv );
        gl_FragColor = f_diffuse * tex_color0;
        \n#endif\n
    }); */

    std::vector<ProgramUniform> gPosColorUVUniforms3D = {ProgramUniform("pd_modelMatrix", UniformDataType::eMatrix4),
        ProgramUniform("pd_diffuse", UniformDataType::eVector4)}; 
}

#endif // PD_POSCOLORUV_SHADER_3D_H
