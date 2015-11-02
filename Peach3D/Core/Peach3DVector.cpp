//
//  Peach3DVector4.h
//  TestPeach3D
//
//  Created by singoon he on 12-7-4.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#include "Peach3DVector2.h"
#include "Peach3DVector3.h"
#include "Peach3DVector4.h"
#include "Peach3DMatrix4.h"

namespace Peach3D
{
    Vector3 Vector3::operator*(const Matrix4& other) const
    {
        Vector3 ov;
        ov.x = x * other.mat[0] + y * other.mat[4] + z * other.mat[8] + other.mat[12];
        ov.y = x * other.mat[1] + y * other.mat[5] + z * other.mat[9] + other.mat[13];
        ov.z = x * other.mat[2] + y * other.mat[6] + z * other.mat[10] + other.mat[14];
        return ov;
    }
    
    Vector4 Vector4::operator*(const Matrix4& other) const
    {
        Vector4 ov;
        ov.x = x * other.mat[0] + y * other.mat[4] + z * other.mat[8] + w * other.mat[12];
        ov.y = x * other.mat[1] + y * other.mat[5] + z * other.mat[9] + w * other.mat[13];
        ov.z = x * other.mat[2] + y * other.mat[6] + z * other.mat[10] + w * other.mat[14];
        ov.w = x * other.mat[3] + y * other.mat[7] + z * other.mat[11] + w * other.mat[15];
        return ov;
    }
}
