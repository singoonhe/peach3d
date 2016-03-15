//
//  Peach3DVector3.h
//  TestPeach3D
//
//  Created by singoon he on 12-7-4.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#ifndef PEACH3D_VECTOR3_H
#define PEACH3D_VECTOR3_H

#include "Peach3DCompile.h"
#include "Peach3DVector2.h"

namespace Peach3D
{
    class Matrix4;
    class PEACH3D_DLL Vector3
    {
    public:
        Vector3(float item = 0.f) { x = y = z = item; }
        //! Constructor with null params.
        Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
        //! Contructor with another Vector2.
        Vector3(const Vector2& other, float _z=1.0f) : x(other.x), y(other.y), z(_z) {}
        //! Contructor with another Vector3.
        Vector3(const Vector3& other) : x(other.x), y(other.y), z(other.z) {}
        
        //! Operators
        Vector3 operator-() const { return Vector3(-x, -y, -z); }
        Vector3 &operator=(const Vector3& other) { x = other.x; y = other.y; z = other.z; return *this; }
        Vector3 operator+(const Vector3& other) const { return Vector3(x + other.x, y + other.y, z + other.z); }
        Vector3 operator-(const Vector3& other) const { return Vector3(x - other.x, y - other.y, z - other.z); }
        Vector3 operator*(float value) const { return Vector3(x * value, y * value, z * value); }
        Vector3 operator*(const Vector3& other) const { return Vector3(x * other.x, y * other.y, z * other.z); }
        Vector3 operator/(float value) const { return Vector3(x / value, y / value, z / value); }
        Vector3 operator/(const Vector3& other) const { return Vector3(x / other.x, y / other.y, z / other.z); }
        bool operator==(const Vector3& other) const
        {
            return FLOAT_EQUAL(x, other.x) && FLOAT_EQUAL(y, other.y) && FLOAT_EQUAL(z, other.z);
        }
        bool operator!=(const Vector3& other) const
        {
            return !FLOAT_EQUAL(x, other.x) || !FLOAT_EQUAL(y, other.y) || !FLOAT_EQUAL(z, other.z);
        }
        //! extend to vec4 and multiply with matrix4
        Vector3 operator*(const Matrix4& other) const;
        
        //! Calculate dot from other Vector3.
        float dot(const Vector3& other) const { return x*other.x + y*other.y + z*other.z; }
        //! Calculate distance from other Vector3.
        float distance(const Vector3& other) const { return sqrt(pow(x - other.x, 2) + pow(y - other.y, 2) + pow(z - other.z, 2)); };
        //! Get projection to another Vector3.
        Vector3 projective(const Vector3& other) const { return other*((dot(other)) / (pow(other.x, 2) + pow(other.y, 2) + pow(other.z, 2))); }
        //! Get vector3 length.
        float length() const { return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)); }
        //! Get cross with another Vector3.
        Vector3 cross(const Vector3& other) const { return Vector3(y*other.z - z*other.y, z*other.x - x*other.z, x*other.y - y*other.x); }
        //! Normalize Vector3.
        void normalize()
        {
            float length = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
            if (length > 0.0f)
            {
                x /= length; y /= length; z /= length;
            }
        }
        
    public:
        float x, y, z;
    };
    //! Define zero vector3.
    const Vector3 Vector3Zero;
}

#endif // PEACH3D_VECTOR_H
