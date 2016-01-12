//
//  Peach3DOBB.h
//  Peach3DLib
//
//  Created by singoon.he on 1/12/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#ifndef PEACH3D_OBB_H
#define PEACH3D_OBB_H

#include "Peach3DVector3.h"
#include "Peach3DMatrix4.h"
#include "Peach3DColor4.h"

namespace Peach3D
{
    class PEACH3D_DLL OBB
    {
    public:
        OBB() {}
        OBB(const OBB& _aabb) {min = _aabb.min; max = _aabb.max;}
        OBB(const Vector3& _min, const Vector3& _max) { min = _min; max = _max; }
        OBB &operator=(const OBB& other) { min = other.min; max = other.max; return *this; }
        
        /** If max <= min, it not valid. */
        bool isValid() {return !(min.x > max.x || min.y > max.y || min.z > max.z);}
        /** Check is Ray intersect. */
        //bool isRayIntersect(const Ray& ray);
        
        void setModelMatrix(const Matrix4& mat) { mModelMat = mat; }
        const Matrix4& getModelMatrix() { return mModelMat; }
        void setDiffuse(const Color4& color) { mDiffuse = color; }
        const Color4& getDiffuse() { return mDiffuse; }
        
    public:
        Vector3 min;    // min position
        Vector3 max;    // max position
        
        Matrix4 mModelMat;  // OBB translate same as Object
        Color4  mDiffuse;   // OBB line color
    };
}

#endif /* PEACH3D_OBB_H */
