//
//  Peach3DShaderCode.h
//  Peach3DLib
//
//  Created by singoon.he on 7/1/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#ifndef PEACH3D_SHADER_CODE_H
#define PEACH3D_SHADER_CODE_H

#include "Peach3DCompile.h"

namespace Peach3D
{
    // user uniform type, texture not need add to uniform list
    enum class PEACH3D_DLL UniformDataType
    {
        eFloat,
        eVector2,
        eVector3,
        eVector4,
        eMatrix4,
    };
    
    enum class PEACH3D_DLL UniformNameType
    {
        eUnknow,        // unknow uniform name, used for user program
        eViewRect,      // Current viewport rect
        eShowRect,      // widget render render pos under anchor and size
        eAnRot,         // widget anchor and rotate
        ePatShowRect,   // widget parent render pos under anchor and size
        ePatAnRot,      // widget parent anchor and rotate
        eDiffuse,       // widget diffuse color or material diffuse
        eUVRect,        // sprite texture rect
        eTexEffect,     // sprite scale negative and gray effect
        
        eProjMatrix,    // projective matrix
        eViewMatrix,    // view matrix
        eModelMatrix,   // model matrix
        eNormalMatrix,  // normal matrix
        eMatEmissive,   // material emissive
        eMatSpecular,   // material specular, include shininess
        eMatAmbient,    // material ambient
        
        eLightTypeSpot, // light type and spot light extend attenute
        eLightPos,      // light position
        eLightDir,      // light direction
        eLightAtten,    // light attenute
        eLightAmbient,  // light ambient
        eLightColor,    // light color
        eEyeDir,       // camera direction
    };
    
    // defined object render uniform desc
    struct PEACH3D_DLL ProgramUniform
    {
        ProgramUniform() {}
        ProgramUniform(const std::string& _name, UniformDataType _type, uint _offset = 0) : name(_name), dType(_type), offset(_offset) {}
        std::string     name;   // uniform name
        UniformDataType dType;  // uniform type, float/vec2/vec3/matrix4...
        uint            offset; // current uniform offset, uniforms may not closely packed
    };
    
    // preset program params
    struct PEACH3D_DLL PresetProgramFeatures
    {
        PresetProgramFeatures(bool _point3, bool _texUV=false, int _lcount=0) :isPoint3(_point3), isTexUV(_texUV), lightsCount(_lcount) {}
        bool    isPoint3;       // 3D node or widget
        bool    isTexUV;        // is contain texture for UV
        int     lightsCount;    // lights count for 3D node
    };
    
    class ShaderCode
    {
    public:
        /* Return shader code by name */
        const std::string& getShaderCode(bool isVertex, const PresetProgramFeatures& feature);
        /* Return shader uniforms by name */
        const std::vector<ProgramUniform>& getProgramUniforms(const PresetProgramFeatures& feature);
        /* Calc cache name from program feature.*/
        std::string getNameOfProgramFeature(bool isVertex, const PresetProgramFeatures& feature);
        /* Calc vertex type from program feature.*/
        uint getVerTypeOfProgramFeature(const PresetProgramFeatures& feature);
        
        /* Return shader uniforms by name */
        static uint getUniformFloatBits(UniformDataType type);
        /* Return shader uniform name by type */
        static UniformNameType getUniformNameType(const std::string& name);
        
    public:
        std::map<std::string, std::string> mShaderMap;
        std::map<std::string, std::vector<ProgramUniform>> mUniformsMap;
        static std::map<UniformDataType, uint> mUniformsBitsMap;
        static std::map<std::string, UniformNameType> mUniformsNamesMap;
    };
}

#endif /* defined(PEACH3D_SHADER_CODE_H) */
