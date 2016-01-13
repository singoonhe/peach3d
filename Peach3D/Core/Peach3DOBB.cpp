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
        mModelMat = mTranslateMat * translate * rotate * scale * mScaleMat;
    }
    
    void OBB::calcCacheMatrix()
    {
        Vector3 size = Vector3(max.x - min.x, max.y - min.y, max.z - min.z);
        Vector3 center = Vector3((max.x + min.x) / 2.0f, (max.y + min.y) / 2.0f, (max.z + min.z) / 2.0f);
        mTranslateMat = Matrix4::createTranslation(center.x, center.y, center.z);
        mScaleMat = Matrix4::createScaling(size.x, size.y, size.z);
    }
}