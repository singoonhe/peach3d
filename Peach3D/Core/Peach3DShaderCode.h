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
        eEyeDir,        // camera direction
        
        eShadowMatrix,  // shadow matrix
        eBoneMatrix,    // bone matrix
        
        eDetailSize,    // terrain texture detail size
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
    
    /* defined all feature name for program */
#define PROGRAM_FEATURE_POINT3      "Point3"    // 3D node or widget, must be included
#define PROGRAM_FEATURE_UV          "UV"        // contain texture for UV
#define PROGRAM_FEATURE_PARTICLE    "Particle"  // used for particle
#define PROGRAM_FEATURE_LIGHT       "Light"     // lights count for 3D node
#define PROGRAM_FEATURE_SHADOW      "Shadow"    // shadow texture count for 3D node
#define PROGRAM_FEATURE_BONE        "Bone"      // bone count for 3d node
#define PROGRAM_FEATURE_TERRAIN     "Terrain"   // terrain texture count
    // use list contain features
    typedef std::map<std::string, int> ProgramFeatureMap;
    
    class ShaderCode
    {
    public:
        /* Return shader code by name */
        const std::string& getShaderCode(bool isVertex, const ProgramFeatureMap& feature);
        /* Return shader uniforms by name */
        const std::vector<ProgramUniform>& getProgramUniforms(const ProgramFeatureMap& feature);
        /* Calc cache name from program feature.*/
        std::string getNameOfProgramFeature(bool isVertex, const ProgramFeatureMap& feature);
        /* Calc vertex type from program feature.*/
        uint getVerTypeOfProgramFeature(const ProgramFeatureMap& feature);
        
        /* Return shader uniforms by name */
        static uint getUniformFloatBits(UniformDataType type);
        /* Return shader uniform name by type */
        static UniformNameType getUniformNameType(const std::string& name);
        /* Return shader uniform name by type. */
        static bool isContainTypeFeature(const ProgramFeatureMap& features, const std::string& feat);
        /* Return count of feature, such as light/shadow/bones. */
        static int getCountOfTypeFeature(const ProgramFeatureMap& features, const std::string& feat);
        
    public:
        std::map<std::string, std::string> mShaderMap;
        std::map<std::string, std::vector<ProgramUniform>> mUniformsMap;
        static std::map<UniformDataType, uint> mUniformsBitsMap;
        static std::map<std::string, UniformNameType> mUniformsNamesMap;
        
        static std::vector<ProgramUniform>  mLightUniforms;
    };
}

#endif /* defined(PEACH3D_SHADER_CODE_H) */
