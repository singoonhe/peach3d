//
//  Peach3DMatrix4.h
//  TestPeach3D
//
//  Created by singoon he on 12-7-8.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#ifndef PEACH3D_MATRIX4_H
#define PEACH3D_MATRIX4_H

#include "Peach3DCompile.h"

namespace Peach3D
{
    // Declare other used class.
    class Vector3;
    class Quaternion;

    //! matrix4 for opengl , DX need call transpose
    class PEACH3D_DLL Matrix4
    {
    public:
        Matrix4()
        {
            memcpy(mat, identityMat, sizeof(float)*16);
        }
        Matrix4(const Matrix4& other)
        {
            memcpy(mat, other.mat, sizeof(float)*16);
        }

        //! Operators
        Matrix4 &operator=(const Matrix4& other)
        {
            memcpy(mat, other.mat, sizeof(float)*16);
            return *this;
        }
        Matrix4 operator*(const Matrix4& other) const
        {
            Matrix4 mm;
            const float *m1 = mat, *m2 = other.mat;
            
            mm.mat[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2] + m1[12] * m2[3];
            mm.mat[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2] + m1[13] * m2[3];
            mm.mat[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2] + m1[14] * m2[3];
            mm.mat[3] = m1[3] * m2[0] + m1[7] * m2[1] + m1[11] * m2[2] + m1[15] * m2[3];
            
            mm.mat[4] = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6] + m1[12] * m2[7];
            mm.mat[5] = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6] + m1[13] * m2[7];
            mm.mat[6] = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6] + m1[14] * m2[7];
            mm.mat[7] = m1[3] * m2[4] + m1[7] * m2[5] + m1[11] * m2[6] + m1[15] * m2[7];
            
            mm.mat[8] = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10] + m1[12] * m2[11];
            mm.mat[9] = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10] + m1[13] * m2[11];
            mm.mat[10] = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10] + m1[14] * m2[11];
            mm.mat[11] = m1[3] * m2[8] + m1[7] * m2[9] + m1[11] * m2[10] + m1[15] * m2[11];
            
            mm.mat[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12] * m2[15];
            mm.mat[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13] * m2[15];
            mm.mat[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15];
            mm.mat[15] = m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15];
            
            return mm;
        }

        void identity()
        {
            memcpy(mat, identityMat, sizeof(float)*16);
        }
        
        bool isIdentity()
        {
            return (memcmp(identityMat, mat, sizeof(float)*16) == 0);
        }
        //! transpose, dx should use it
        Matrix4 getTranspose() const
        {
            Matrix4 mm;
            for (auto i = 0; i < 4; ++i)
            {
                for (auto j = 0; j < 4; ++j)
                {
                    mm.mat[(i * 4) + j] = mat[(j * 4) + i];
                }
            }
            return mm;
        }
        /**
         * @brief Calc inverse Matrix4 from current mat using gauss algorithm.
         * @params outMat If outMat valid, return result mat.
         * @return Is inverse mat existed.
         */
        bool getInverse(Matrix4* outMat);
        
        //! get vector info in matrix4
        Vector3 getUpVec3();
        Vector3 getRightVec3();
        Vector3 getForwardVec3();
        
        //! create matrix with more params
        static Matrix4 createRotationAxis(const Vector3& axis, float radians);
        static Matrix4 createRotationX(float radians);
        static Matrix4 createRotationY(float radians);
        static Matrix4 createRotationZ(float radians);
        static Matrix4 createRotationPitchYawRoll(float pitch, float yaw, float roll);
        static Matrix4 createRotation2D(const Vector3& radianV, float asPect);
        /** Quickly than createRotationPitchYawRoll. */
        static Matrix4 createRotationQuaternion(const Quaternion& q);
        static Matrix4 createScaling(float x, float y, float z);
        static Matrix4 createTranslation(float x, float y, float z);
        static Matrix4 createPerspectiveProjection(float fovY, float asPect, float zNear, float zFar);
        static Matrix4 createOrthoProjection(float left, float right, float bottom, float top, float nearVal, float farVal);
        static Matrix4 createLookAt(const Vector3& eye, const Vector3& center, const Vector3& up);

    public:
        float mat[16];
        static float identityMat[16];
    };
}

#endif // PEACH3D_MATRIX4_H
