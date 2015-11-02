//
//  Peach3DAABB.cpp
//  Peach3DLib
//
//  Created by singoon.he on 4/3/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#include "Peach3DAABB.h"

namespace Peach3D
{
    bool AABB::isRayIntersect(const Ray& ray)
    {
        // Algorithm come frome "Woo"
        bool inside = true;
        float xt;
        if (ray.startPos.x < min.x) {
            xt = min.x - ray.startPos.x;
            if (xt > ray.delta.x) {
                return false;
            }
            xt /= ray.delta.x;
            inside = false;
        }
        else if (ray.startPos.x > max.x) {
            xt = max.x - ray.startPos.x;
            if (xt < ray.delta.x) {
                return false;
            }
            xt /= ray.delta.x;
            inside = false;
        }
        else {
            xt = -1.0f;
        }

        float yt;
        if (ray.startPos.y < min.y) {
            yt = min.y - ray.startPos.y;
            if (yt > ray.delta.y) {
                return false;
            }
            yt /= ray.delta.y;
            inside = false;
        }
        else if (ray.startPos.y > max.y) {
            yt = max.y - ray.startPos.y;
            if (yt < ray.delta.y) {
                return false;
            }
            yt /= ray.delta.y;
            inside = false;
        }
        else {
            yt = -1.0f;
        }

        float zt;
        if (ray.startPos.z < min.z) {
            zt = min.z - ray.startPos.z;
            if (zt > ray.delta.z) {
                return false;
            }
            zt /= ray.delta.z;
            inside = false;
        }
        else if (ray.startPos.z > max.z) {
            zt = max.z - ray.startPos.z;
            if (zt < ray.delta.z) {
                return false;
            }
            zt /= ray.delta.z;
            inside = false;
        }
        else {
            zt = -1.0f;
        }
       
        if (inside ) {
            return true;
        }

        int which = 0;
        float t = xt;
        if (yt > t) {
            which = 1;
            t = yt;
        }
        if (zt > t) {
            which = 2;
            t = zt;
        }
        switch (which)
        {
        case 0: // check yz plane
        {
            float y = ray.startPos.y + ray.delta.y * t;
            if (y < min.y || y > max.y) {
                return false;
            }
            float z = ray.startPos.z + ray.delta.z * t;
            if (z < min.z || z > max.z) {
                return false;
            }
            break;
        }
        case 1: // check xz plane
        {
            float x = ray.startPos.x + ray.delta.x * t;
            if (x < min.x || x > max.x) {
                return false;
            }
            float z = ray.startPos.z + ray.delta.z * t;
            if (z < min.z || z > max.z) {
                return false;
            }
            break;
        }
        case 2: // check xy plane
        {
            float x = ray.startPos.x + ray.delta.x * t;
            if (x < min.x || x > max.x) {
                return false;
            }
            float y = ray.startPos.y + ray.delta.y * t;
            if (y < min.y || y > max.y) {
                return false;
            }
            break;
        }
        default:
            break;
        }
        return t>=0.0f && t<=1.0f;
    }
}