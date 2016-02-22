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
#include "Peach3DColor4.h"

namespace Peach3D
{
    enum class PEACH3D_DLL LightType
    {
        eUnknow,    // used for initialize light
        eDirection, // direction light
        ePoint,     // point light
        eSpot,      // spot light
    };
    
    struct PEACH3D_DLL Light
    {
        Light() : type(LightType::eUnknow) {}
        // constructor for direction light
        Light(const Vector3& _dir) : type(LightType::eDirection), dir(_dir) {}
        
    public:
        LightType   type;   // light type
        Vector3     pos;    // light position
        Vector3     dir;    // light direction
        Color3      color;  // light color
    };
}

#endif /* PEACH3D_LIGHT_H */
