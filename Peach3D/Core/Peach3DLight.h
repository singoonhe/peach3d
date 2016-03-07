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
        // constructor for direction light
        Light(const Vector3& _dir, const Color3& _color, const Color3& _ambient = Color3(0.1f, 0.1f, 0.1f)) : type(LightType::eDirection), color(_color), ambient(_ambient) { dir = _dir; dir.normalize(); }
        // constructor for dot light
        Light(const Vector3& _pos, const Vector3& _attenuate = Vector3(2.f, 10.f, 5.f), const Color3& _color = Color3(1.f, 1.f, 1.f), const Color3& _ambient = Color3(0.1f, 0.1f, 0.1f)) : type(LightType::eDot), pos(_pos), attenuate(_attenuate), color(_color), ambient(_ambient) {}
        // constructor for spot light
        Light(const Vector3& _pos, const Vector3& _dir, const Vector2& _ext = Vector2(0.7f, 5.f), const Vector3& _attenuate = Vector3(2.f, 10.f, 5.f), const Color3& _color = Color3(1.f, 1.f, 1.f), const Color3& _ambient = Color3(0.1f, 0.1f, 0.1f)) : type(LightType::eSpot), pos(_pos), spotExt(_ext), attenuate(_attenuate), color(_color), ambient(_ambient) { dir = _dir; dir.normalize(); }
                
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
