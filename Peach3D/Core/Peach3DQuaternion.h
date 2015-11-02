//
//  Peach3DQuaternion.h
//  TestPeach3D
//
//  Created by singoon.he on 12-8-13.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#ifndef PEACH3D_QUATERNION_H
#define PEACH3D_QUATERNION_H

#include "Peach3DCompile.h"
#include "Peach3DVector3.h"

namespace Peach3D
{
    class PEACH3D_DLL Quaternion
    {
    public:
        //! Constructor with zero params.
        Quaternion() { w = 1.0f; x = 0.0f; y = 0.0f; z = 0.0f; }
        //! Constructor with four params.
        Quaternion(float qw, float qx, float qy, float qz) { w = qw; x = qx; y = qy; z = qz; }
        //! Constructor with another Quaternion.
        Quaternion(const Quaternion& other) { w = other.w; x = other.x; y = other.y; z = other.z; }

        //! Operators
        Quaternion operator-() const { return Quaternion(w, -x, -y, -z); }
        Quaternion &operator=(const Quaternion& other) { w = other.w; x = other.x; y = other.y; z = other.z; return *this; }
        Quaternion operator*(const Quaternion& other) const
        {
            Quaternion result;
            result.w = w*other.w - x*other.x - y*other.y - z*other.z;
            result.x = w*other.x + x*other.w + z*other.y - y*other.z;
            result.y = w*other.y + y*other.w + x*other.z - z*other.x;
            result.z = w*other.z + z*other.w + y*other.x - x*other.y;
            return result;
        }
        bool operator==(const Quaternion& other) const
        {
            return FLOAT_EQUAL(x, other.x) && FLOAT_EQUAL(y, other.y) && FLOAT_EQUAL(z, other.z) && FLOAT_EQUAL(w, other.w);
        }
        bool operator!=(const Quaternion& other) const
        {
            return !FLOAT_EQUAL(x, other.x) || !FLOAT_EQUAL(y, other.y) || !FLOAT_EQUAL(z, other.z) || !FLOAT_EQUAL(w, other.w);
        }

        //! Dot with another Quaternion.
        float dot(const Quaternion& other)
        {
            return other.w*w + other.x*x + other.y*y + other.z*z;
        }
        //! Normalize Quaternion.
        void normalize();
        //! Calculate the Quaternion's pow.
        Quaternion pow(float exponent);
        //! Slerp with another Quaternion by t.
        Quaternion slerp(const Quaternion& dist, float t);

        //! Init with rotation.
        static Quaternion createByRotateWithX(float theta);
        static Quaternion createByRotateWithY(float theta);
        static Quaternion createByRotateWithZ(float theta);
        static Quaternion createByRotateWithAxis(const Vector3& axis, float theta);
        static Quaternion createByEulerAngle(const Vector3& ea);
        //! Get Quaternion angle and axis.
        float getRotationAngle() const;
        Vector3 getRotationAxis() const;
        Vector3 getEulerAngle();

    public:
        float w, x, y, z;
    };

    //! Define identity quaternion.
    const Quaternion QuaternionIdentity;
}

#endif // PEACH3D_QUATERNION_H
