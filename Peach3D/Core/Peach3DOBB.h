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
        OBB(const OBB& _aabb) {min = _aabb.min; max = _aabb.max; calcCacheMatrix();}
        OBB(const Vector3& _min, const Vector3& _max) { min = _min; max = _max; calcCacheMatrix();}
        OBB &operator=(const OBB& other) { min = other.min; max = other.max; calcCacheMatrix(); return *this; }
        
        /** If max <= min, it not valid. */
        bool isValid() {return !(min.x > max.x || min.y > max.y || min.z > max.z);}
        /** Check is Ray intersect. */
        //bool isRayIntersect(const Ray& ray);
        
        void setModelMatrix(const Matrix4& translate, const Matrix4& rotate, const Matrix4& scale);
        const Matrix4& getModelMatrix() { return mModelMat; }
        
    private:
        /** Calc cache scale matrix and translate matrix. */
        void calcCacheMatrix();
        
    private:
        Vector3 min;    // min position
        Vector3 max;    // max position
        
        Matrix4 mScaleMat;      // OBB cache scale mat
        Matrix4 mTranslateMat;  // OBB cache translate mat
        Matrix4 mModelMat;      // OBB translate same as Object
    };
}

#endif /* PEACH3D_OBB_H */
