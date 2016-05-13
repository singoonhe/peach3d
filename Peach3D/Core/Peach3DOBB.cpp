//
//  Peach3DOBB.cpp
//  Peach3DLib
//
//  Created by singoon.he on 1/12/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "Peach3DOBB.h"

namespace Peach3D
{
    void OBB::setModelMatrix(const Matrix4& translate, const Matrix4& rotate, const Matrix4& scale)
    {
        mModelMat = translate * rotate * scale * mLocalMat;
    }
    
    void OBB::calcCacheMatrix(const Vector3& min, const Vector3& max)
    {
        Vector3 size = Vector3(max.x - min.x, max.y - min.y, max.z - min.z);
        Vector3 center = Vector3((max.x + min.x) / 2.0f, (max.y + min.y) / 2.0f, (max.z + min.z) / 2.0f);
        auto translateMat = Matrix4::createTranslation(center);
        auto scaleMat = Matrix4::createScaling(size);
        mLocalMat = translateMat * scaleMat;
    }
    
    bool OBB::isRayIntersect(const Ray& ray)
    {
        Matrix4 inverseMat;
        Vector3 min(-0.5f, -0.5f, -0.5f), max(0.5f, 0.5f, 0.5f);
        if (mModelMat.getInverse(&inverseMat)) {
            // convert ray to OBB object base coordinate(defined in object vertex)
            Vector3 pos = ray.startPos * inverseMat;
            Vector3 delta = ray.delta * inverseMat;
            
            return checkRayIntersect(pos, delta, min, max);
        }
        else {
            // check OBB is plane
            bool isPlane = false;
            if (FLOAT_EQUAL_0(mModelMat.mat[0])) {
                min.x = max.x = 0.f;
                isPlane = true;
            }
            else if (FLOAT_EQUAL_0(mModelMat.mat[5])) {
                min.y = max.y = 0.f;
                isPlane = true;
            }
            else if (FLOAT_EQUAL_0(mModelMat.mat[10])) {
                min.z = max.z = 0.f;
                isPlane = true;
            }
            
            if (isPlane) {
                // current, convert OBB to model matrix then check.
                // TODO: there may need a good algorithm
                min = min * mModelMat;
                max = max * mModelMat;
                return checkRayIntersect(ray.startPos, ray.delta, min, max);
            }
        }
        return false;
    }
    
    bool OBB::checkRayIntersect(const Vector3& pos, const Vector3& delta, const Vector3& min, const Vector3& max)
    {
        // Algorithm come frome "Woo"
        bool inside = true;
        float xt;
        if (pos.x < min.x) {
            xt = min.x - pos.x;
            if (xt > delta.x) {
                return false;
            }
            xt /= delta.x;
            inside = false;
        }
        else if (pos.x > max.x) {
            xt = max.x - pos.x;
            if (xt < delta.x) {
                return false;
            }
            xt /= delta.x;
            inside = false;
        }
        else {
            xt = -1.0f;
        }
        
        float yt;
        if (pos.y < min.y) {
            yt = min.y - pos.y;
            if (yt > delta.y) {
                return false;
            }
            yt /= delta.y;
            inside = false;
        }
        else if (pos.y > max.y) {
            yt = max.y - pos.y;
            if (yt < delta.y) {
                return false;
            }
            yt /= delta.y;
            inside = false;
        }
        else {
            yt = -1.0f;
        }
        
        float zt;
        if (pos.z < min.z) {
            zt = min.z - pos.z;
            if (zt > delta.z) {
                return false;
            }
            zt /= delta.z;
            inside = false;
        }
        else if (pos.z > max.z) {
            zt = max.z - pos.z;
            if (zt < delta.z) {
                return false;
            }
            zt /= delta.z;
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
                float y = pos.y + delta.y * t;
                if (y < min.y || y > max.y) {
                    return false;
                }
                float z = pos.z + delta.z * t;
                if (z < min.z || z > max.z) {
                    return false;
                }
                break;
            }
            case 1: // check xz plane
            {
                float x = pos.x + delta.x * t;
                if (x < min.x || x > max.x) {
                    return false;
                }
                float z = pos.z + delta.z * t;
                if (z < min.z || z > max.z) {
                    return false;
                }
                break;
            }
            case 2: // check xy plane
            {
                float x = pos.x + delta.x * t;
                if (x < min.x || x > max.x) {
                    return false;
                }
                float y = pos.y + delta.y * t;
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