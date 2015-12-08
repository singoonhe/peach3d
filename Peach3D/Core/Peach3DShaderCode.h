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
        eDiffuse,       // widget diffuse color or SceneNode diffuse
        eUVRect,        // sprite texture rect
        eTexEffect,     // sprite scale negative and gray effect
        
        eProjMatrix,    // SceneNode projective matrix
        eViewMatrix,    // SceneNode view matrix
        eModelMatrix,   // SceneNode model matrix
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
    
    // defined shader code data
    struct PEACH3D_DLL ShaderCodeData
    {
        ShaderCodeData() {data=nullptr; size=0;}
        ShaderCodeData(char* _data, int _size) : data(_data), size(_size) {}
        char*   data;   // shader code data pointer
        int     size;   // shader code data size
    };
    
    class ShaderCode
    {
    public:
        /* Return shader code by name */
        const ShaderCodeData& getShaderCode(const std::string& name);
        /* Return shader uniforms by name */
        const std::vector<ProgramUniform>& getProgramUniforms(const std::string& name);
        
        /* Return shader uniforms by name */
        static uint getUniformFloatBits(UniformDataType type);
        /* Return shader uniform name by type */
        static UniformNameType getUniformNameType(const std::string& name);
        
        /* Free all shader code, called in ResourceManager. */
        void freeShaderCodeData();
    public:
        std::map<std::string, ShaderCodeData> mShaderMap;
        std::map<std::string, std::vector<ProgramUniform>> mUniformsMap;
        static std::map<UniformDataType, uint> mUniformsBitsMap;
        static std::map<std::string, UniformNameType> mUniformsNamesMap;
    };
}

#endif /* defined(PEACH3D_SHADER_CODE_H) */
