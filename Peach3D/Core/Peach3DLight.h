//
//  Peach3DLight.h
//  Peach3DLib
//
//  Created by singoon.he on 2/22/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#ifndef PEACH3D_LIGHT_H
#define PEACH3D_LIGHT_H

#include "Peach3DCompile.h"
#include "Peach3DVector2.h"
#include "Peach3DVector3.h"
#include "Peach3DColor4.h"

namespace Peach3D
{
    enum class PEACH3D_DLL LightType
    {
        eUnknow,    // used for initialize light
        eDirection, // direction light
        eDot,       // dot light
        eSpot,      // spot light
    };
    
    struct PEACH3D_DLL Light
    {
        Light() : type(LightType::eUnknow) {}
        /** Constructor for direction light with direction and color. */
        Light(const Vector3& _dir, const Color3& _color, const Color3& _ambient = Color3Gray) : type(LightType::eDirection), color(_color), ambient(_ambient) { dir = _dir; dir.normalize(); }
        /** Constructor for dot light.
         * @params _attenuate light attenuation, const/line/square. Example:Vector3(1.f, 0.1f, 0.f).
         */
        Light(const Vector3& _pos, const Vector3& _attenuate, const Color3& _color = Color3White, const Color3& _ambient = Color3Gray) : type(LightType::eDot), pos(_pos), attenuate(_attenuate), color(_color), ambient(_ambient) {}
        /** Constructor for spot light.
         * @params _attenuate same as dot light.
         * @params _ext spot light angle cos and transverse attenuate(pow).
         */
        Light(const Vector3& _pos, const Vector3& _dir, const Vector3& _attenuate, const Vector2& _ext = Vector2(0.5f, 2.f), const Color3& _color = Color3White, const Color3& _ambient = Color3Gray) : type(LightType::eSpot), pos(_pos), spotExt(_ext), attenuate(_attenuate), color(_color), ambient(_ambient) { dir = _dir; dir.normalize(); }
        //! Operators
        Light &operator=(const Light& other)
        {
            name = other.name;
            type = other.type;
            pos = other.pos;
            dir = other.dir;
            ambient = other.ambient;
            color = other.color;
            attenuate = other.attenuate;
            spotExt = other.spotExt;
            return *this;
        }
        
    public:
        std::string name;   // light name
        LightType   type;   // light type
        Vector3     pos;    // light position, for Dot and Spot
        Vector3     dir;    // light direction, for Direction and Spot
        Color3      ambient;// light ambient
        Color3      color;  // light color
        Vector3     attenuate;  // light const/line/quadratic attenuate, for Dot and Spot
        Vector2     spotExt;//  extent spot and cut cos attenuate, for Spot
    };
}

#endif /* PEACH3D_LIGHT_H */
