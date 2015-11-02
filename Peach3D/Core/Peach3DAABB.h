//
//  Peach3DAABB.h
//  Peach3DLib
//
//  Created by singoon.he on 4/3/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#ifndef PEACH3D_AABB_H
#define PEACH3D_AABB_H

#include "Peach3DVector3.h"

namespace Peach3D
{
    // define ray struct
    struct PEACH3D_DLL Ray
    {
        Vector3 startPos;   // ray start position
        Vector3 delta;      // ray direction and length
    };
    
    class PEACH3D_DLL AABB
    {
    public:
        AABB() {}
        AABB(const AABB& _aabb) {min = _aabb.min; max = _aabb.max;}
        AABB(const Vector3& _min, const Vector3& _max) { min = _min; max = _max; }
        AABB &operator=(const AABB& other) { min = other.min; max = other.max; return *this; }
        /**
         * If max <= min, it not valid.
         */
        bool isValid() {return !(min.x > max.x || min.y > max.y || min.z > max.z);}
        /**
         * @brief Check is Ray intersect.
         */
        bool isRayIntersect(const Ray& ray);
        
    public:
        Vector3 min;    // min position
        Vector3 max;    // max position
    };
}

#endif /* defined(PEACH3D_AABB_H) */
