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
    // define ray struct
    struct PEACH3D_DLL Ray
    {
        Vector3 startPos;   // ray start position
        Vector3 delta;      // ray direction and length
    };
    
    class PEACH3D_DLL OBB
    {
    public:
        OBB(const OBB& _obb) { mScaleMat = _obb.mScaleMat; mTranslateMat = _obb.mTranslateMat; }
        OBB(const Vector3& _min, const Vector3& _max) { calcCacheMatrix(_min, _max); }
        OBB &operator=(const OBB& other) { mScaleMat = other.mScaleMat; mTranslateMat = other.mTranslateMat; return *this; }
        
        /** Check is Ray intersect. */
        bool isRayIntersect(const Ray& ray);
        
        void setModelMatrix(const Matrix4& translate, const Matrix4& rotate, const Matrix4& scale);
        const Matrix4& getModelMatrix() { return mModelMat; }
        
    private:
        /** Calc cache scale matrix and translate matrix. */
        void calcCacheMatrix(const Vector3& min, const Vector3& max);
        
    private:
        Matrix4 mScaleMat;      // OBB cache scale mat
        Matrix4 mTranslateMat;  // OBB cache translate mat
        Matrix4 mModelMat;      // OBB translate same as Object
    };
}

#endif /* PEACH3D_OBB_H */
