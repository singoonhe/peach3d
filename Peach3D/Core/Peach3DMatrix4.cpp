//
//  Peach3DMatrix4.cpp
//  TestPeach3D
//
//  Created by singoon.he on 12-8-19.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#include "Peach3DMatrix4.h"
#include "Peach3DVector3.h"
#include "Peach3DQuaternion.h"

namespace Peach3D
{
    // defined identity matrix4, use to copy to orther matrix4
    float Matrix4::identityMat[16]={1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    
    Vector3 Matrix4::getUpVec3()
    {
        Vector3 ov;
        ov.x = mat[4];
        ov.y = mat[5];
        ov.z = mat[6];
        ov.normalize();
        return ov;
    }
    
    Vector3 Matrix4::getRightVec3()
    {
        Vector3 ov;
        ov.x = mat[0];
        ov.y = mat[1];
        ov.z = mat[2];
        ov.normalize();
        return ov;
    }
    
    Vector3 Matrix4::getForwardVec3()
    {
        Vector3 ov;
        ov.x = mat[8];
        ov.y = mat[9];
        ov.z = mat[10];
        ov.normalize();
        return ov;
    }
    
    bool Matrix4::getInverse(Matrix4* outMat)
    {
        double inv[16], det;
        int i;
        
        inv[0] = mat[5]  * mat[10] * mat[15] -
        mat[5]  * mat[11] * mat[14] -
        mat[9]  * mat[6]  * mat[15] +
        mat[9]  * mat[7]  * mat[14] +
        mat[13] * mat[6]  * mat[11] -
        mat[13] * mat[7]  * mat[10];
        
        inv[4] = -mat[4]  * mat[10] * mat[15] +
        mat[4]  * mat[11] * mat[14] +
        mat[8]  * mat[6]  * mat[15] -
        mat[8]  * mat[7]  * mat[14] -
        mat[12] * mat[6]  * mat[11] +
        mat[12] * mat[7]  * mat[10];
        
        inv[8] = mat[4]  * mat[9] * mat[15] -
        mat[4]  * mat[11] * mat[13] -
        mat[8]  * mat[5] * mat[15] +
        mat[8]  * mat[7] * mat[13] +
        mat[12] * mat[5] * mat[11] -
        mat[12] * mat[7] * mat[9];
        
        inv[12] = -mat[4]  * mat[9] * mat[14] +
        mat[4]  * mat[10] * mat[13] +
        mat[8]  * mat[5] * mat[14] -
        mat[8]  * mat[6] * mat[13] -
        mat[12] * mat[5] * mat[10] +
        mat[12] * mat[6] * mat[9];
        
        inv[1] = -mat[1]  * mat[10] * mat[15] +
        mat[1]  * mat[11] * mat[14] +
        mat[9]  * mat[2] * mat[15] -
        mat[9]  * mat[3] * mat[14] -
        mat[13] * mat[2] * mat[11] +
        mat[13] * mat[3] * mat[10];
        
        inv[5] = mat[0]  * mat[10] * mat[15] -
        mat[0]  * mat[11] * mat[14] -
        mat[8]  * mat[2] * mat[15] +
        mat[8]  * mat[3] * mat[14] +
        mat[12] * mat[2] * mat[11] -
        mat[12] * mat[3] * mat[10];
        
        inv[9] = -mat[0]  * mat[9] * mat[15] +
        mat[0]  * mat[11] * mat[13] +
        mat[8]  * mat[1] * mat[15] -
        mat[8]  * mat[3] * mat[13] -
        mat[12] * mat[1] * mat[11] +
        mat[12] * mat[3] * mat[9];
        
        inv[13] = mat[0]  * mat[9] * mat[14] -
        mat[0]  * mat[10] * mat[13] -
        mat[8]  * mat[1] * mat[14] +
        mat[8]  * mat[2] * mat[13] +
        mat[12] * mat[1] * mat[10] -
        mat[12] * mat[2] * mat[9];
        
        inv[2] = mat[1]  * mat[6] * mat[15] -
        mat[1]  * mat[7] * mat[14] -
        mat[5]  * mat[2] * mat[15] +
        mat[5]  * mat[3] * mat[14] +
        mat[13] * mat[2] * mat[7] -
        mat[13] * mat[3] * mat[6];
        
        inv[6] = -mat[0]  * mat[6] * mat[15] +
        mat[0]  * mat[7] * mat[14] +
        mat[4]  * mat[2] * mat[15] -
        mat[4]  * mat[3] * mat[14] -
        mat[12] * mat[2] * mat[7] +
        mat[12] * mat[3] * mat[6];
        
        inv[10] = mat[0]  * mat[5] * mat[15] -
        mat[0]  * mat[7] * mat[13] -
        mat[4]  * mat[1] * mat[15] +
        mat[4]  * mat[3] * mat[13] +
        mat[12] * mat[1] * mat[7] -
        mat[12] * mat[3] * mat[5];
        
        inv[14] = -mat[0]  * mat[5] * mat[14] +
        mat[0]  * mat[6] * mat[13] +
        mat[4]  * mat[1] * mat[14] -
        mat[4]  * mat[2] * mat[13] -
        mat[12] * mat[1] * mat[6] +
        mat[12] * mat[2] * mat[5];
        
        inv[3] = -mat[1] * mat[6] * mat[11] +
        mat[1] * mat[7] * mat[10] +
        mat[5] * mat[2] * mat[11] -
        mat[5] * mat[3] * mat[10] -
        mat[9] * mat[2] * mat[7] +
        mat[9] * mat[3] * mat[6];
        
        inv[7] = mat[0] * mat[6] * mat[11] -
        mat[0] * mat[7] * mat[10] -
        mat[4] * mat[2] * mat[11] +
        mat[4] * mat[3] * mat[10] +
        mat[8] * mat[2] * mat[7] -
        mat[8] * mat[3] * mat[6];
        
        inv[11] = -mat[0] * mat[5] * mat[11] +
        mat[0] * mat[7] * mat[9] +
        mat[4] * mat[1] * mat[11] -
        mat[4] * mat[3] * mat[9] -
        mat[8] * mat[1] * mat[7] +
        mat[8] * mat[3] * mat[5];
        
        inv[15] = mat[0] * mat[5] * mat[10] -
        mat[0] * mat[6] * mat[9] -
        mat[4] * mat[1] * mat[10] +
        mat[4] * mat[2] * mat[9] +
        mat[8] * mat[1] * mat[6] -
        mat[8] * mat[2] * mat[5];
        
        det = mat[0] * inv[0] + mat[1] * inv[4] + mat[2] * inv[8] + mat[3] * inv[12];
        
        if (det == 0)
            return false;
        
        if (outMat) {
            det = 1.0 / det;
            
            for (i = 0; i < 16; i++)
                outMat->mat[i] = float(inv[i] * det);
        }
        return true;
    }
    
    Matrix4 Matrix4::createRotationAxis(const Vector3& axis, float radians)
    {
        Matrix4 mm;
        float rcos = cosf(radians);
        float rsin = sinf(radians);
        
        mm.mat[0] = rcos + axis.x * axis.x * (1 - rcos);
        mm.mat[1] = axis.z * rsin + axis.y * axis.x * (1 - rcos);
        mm.mat[2] = -axis.y * rsin + axis.z * axis.x * (1 - rcos);
        
        mm.mat[4] = -axis.z * rsin + axis.x * axis.y * (1 - rcos);
        mm.mat[5] = rcos + axis.y * axis.y * (1 - rcos);
        mm.mat[6] = axis.x * rsin + axis.z * axis.y * (1 - rcos);
        
        mm.mat[8] = axis.y * rsin + axis.x * axis.z * (1 - rcos);
        mm.mat[9] = -axis.x * rsin + axis.y * axis.z * (1 - rcos);
        mm.mat[10] = rcos + axis.z * axis.z * (1 - rcos);
        return mm;
    }
    
    Matrix4 Matrix4::createRotationX(float radians)
    {
        /*
                  |  1  0       0       0 |
          M = |  0  cos(A) -sin(A)  0 |
              |  0  sin(A)  cos(A)  0 |
              |  0  0       0       1 |
        
         */
        
        Matrix4 mm;
        mm.mat[5] = cosf(radians);
        mm.mat[6] = sinf(radians);
        mm.mat[9] = -sinf(radians);
        mm.mat[10] = cosf(radians);
        return mm;
    }

    Matrix4 Matrix4::createRotationY(float radians)
    {
        /*
              |  cos(A)  0   sin(A)  0 |
          M = |  0       1   0       0 |
              | -sin(A)  0   cos(A)  0 |
              |  0       0   0       1 |
         */
        
        Matrix4 mm;
        mm.mat[0] = cosf(radians);
        mm.mat[2] = -sinf(radians);
        mm.mat[8] = sinf(radians);
        mm.mat[10] = cosf(radians);
        return mm;
    }

    Matrix4 Matrix4::createRotationZ(float radians)
    {
        /*
              |  cos(A)  -sin(A)   0   0 |
          M = |  sin(A)   cos(A)   0   0 |
              |  0        0        1   0 |
              |  0        0        0   1 |
         */
       
        Matrix4 mm;
        mm.mat[0] = cosf(radians);
        mm.mat[1] = sinf(radians);
        mm.mat[4] = -sinf(radians);
        mm.mat[5] = cosf(radians);
        return mm;
    }
    
    Matrix4 Matrix4::createRotationPitchYawRoll(const Vector3& pyr)
    {
        double cr = cos(pyr.x);
        double sr = sin(pyr.x);
        double cp = cos(pyr.y);
        double sp = sin(pyr.y);
        double cy = cos(pyr.z);
        double sy = sin(pyr.z);
        double srsp = sr * sp;
        double crsp = cr * sp;
        
        Matrix4 mm;
        mm.mat[0] = float(cp * cy);
        mm.mat[4] = float(cp * sy);
        mm.mat[8] = float(-sp);
        
        mm.mat[1] = float(srsp * cy - cr * sy);
        mm.mat[5] = float(srsp * sy + cr * cy);
        mm.mat[9] = float(sr * cp);
        
        mm.mat[2] = float(crsp * cy + sr * sy);
        mm.mat[6] = float(crsp * sy - sr * cy);
        mm.mat[10] = float(cr * cp);
        return mm;
    }
    
    Matrix4 Matrix4::createRotation2D(const Vector3& radianV, float asPect)
    {
        double cr = cos(radianV.x);
        double sr = sin(radianV.x);
        double cp = cos(radianV.y);
        double sp = sin(radianV.y);
        double cy = cos(radianV.z);
        double sy = sin(radianV.z);
        double srsp = sr * sp;
        double crsp = cr * sp;
        
        Matrix4 mm;
        mm.mat[0] = float(cp * cy);
        mm.mat[4] = float(cp * sy);
        mm.mat[8] = float(-sp);
        
        mm.mat[1] = float(srsp * cy - cr * sy) * asPect;
        mm.mat[5] = float(srsp * sy + cr * cy) * asPect;
        mm.mat[9] = float(sr * cp);
        
        mm.mat[2] = float(crsp * cy + sr * sy);
        mm.mat[6] = float(crsp * sy - sr * cy);
        mm.mat[10] = float(cr * cp);
        return mm;
    }

    Matrix4 Matrix4::createRotationQuaternion(const Quaternion& q)
    {
        Matrix4 mm;
        mm.mat[ 0] = 1.0f - 2.0f * (q.y * q.y + q.z * q.z );
        mm.mat[ 4] = 2.0f * (q.x * q.y + q.z * q.w);
        mm.mat[ 8] = 2.0f * (q.x * q.z - q.y * q.w);
        
        // Second row
        mm.mat[ 1] = 2.0f * ( q.x * q.y - q.z * q.w );
        mm.mat[ 5] = 1.0f - 2.0f * ( q.x * q.x + q.z * q.z );
        mm.mat[ 9] = 2.0f * (q.z * q.y + q.x * q.w );
        
        // Third row
        mm.mat[ 2] = 2.0f * ( q.x * q.z + q.y * q.w );
        mm.mat[ 6] = 2.0f * ( q.y * q.z - q.x * q.w );
        mm.mat[10] = 1.0f - 2.0f * ( q.x * q.x + q.y * q.y );
        return mm;
    }
    
    Matrix4 Matrix4::createScaling(const Vector3& scale)
    {
        Matrix4 mm;
        mm.mat[0] = scale.x;
        mm.mat[5] = scale.y;
        mm.mat[10] = scale.z;
        return mm;
    }
    
    Matrix4 Matrix4::createTranslation(const Vector3& translate)
    {
        Matrix4 mm;
        mm.mat[12] = translate.x;
        mm.mat[13] = translate.y;
        mm.mat[14] = translate.z;
        return mm;
    }
    
    Matrix4 Matrix4::createPerspectiveProjection(float fovY, float asPect, float zNear, float zFar)
    {
        float r = DEGREE_TO_RADIANS(fovY / 2);
        float deltaZ = zFar - zNear;
        float s = sin(r);
        
        Matrix4 mm;
        if (deltaZ == 0 || s == 0 || asPect == 0) {
            return mm;
        }
        
        //cos(r) / sin(r) = cot(r)
        float cotangent = cos(r) / s;
        mm.mat[0] = cotangent / asPect;
        mm.mat[5] = cotangent;
        mm.mat[10] = -(zFar + zNear) / deltaZ;
        mm.mat[11] = -1;
        mm.mat[14] = -2 * zNear * zFar / deltaZ;
        mm.mat[15] = 0;
        return mm;
    }
    
    Matrix4 Matrix4::createOrthoProjection(float left, float right, float bottom, float top, float nearVal, float farVal)
    {
        Matrix4 mm;
        float tx = -((right + left) / (right - left));
        float ty = -((top + bottom) / (top - bottom));
        float tz = -((farVal + nearVal) / (farVal - nearVal));
        
        mm.mat[0] = 2 / (right - left);
        mm.mat[5] = 2 / (top - bottom);
        mm.mat[10] = -2 / (farVal - nearVal);
        mm.mat[12] = tx;
        mm.mat[13] = ty;
        mm.mat[14] = tz;
        return mm;
    }
    
    Matrix4 Matrix4::createLookAt(const Vector3& eye, const Vector3& center, const Vector3& up)
    {
        Vector3 fov = center - eye;
        fov.normalize();
        Vector3 upV = up;
        upV.normalize();
        
        Vector3 sV = fov.cross(up);
        sV.normalize();
        Vector3 uV = sV.cross(fov);
        uV.normalize();
        
        Matrix4 outMat;
        outMat.mat[0] = sV.x;
        outMat.mat[4] = sV.y;
        outMat.mat[8] = sV.z;
        
        outMat.mat[1] = uV.x;
        outMat.mat[5] = uV.y;
        outMat.mat[9] = uV.z;
        
        outMat.mat[2] = -fov.x;
        outMat.mat[6] = -fov.y;
        outMat.mat[10] = -fov.z;
        
        Matrix4 translateM = Matrix4::createTranslation(-eye);
        return outMat * translateM;
    }
}