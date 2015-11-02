//
//  Peach3DVector4.h
//  TestPeach3D
//
//  Created by singoon he on 12-7-4.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#ifndef PEACH3D_VECTOR4_H
#define PEACH3D_VECTOR4_H

#include "Peach3DCompile.h"
#include "Peach3DVector3.h"

namespace Peach3D
{
    class PEACH3D_DLL Vector4
    {
    public:
        Vector4() { x = y = z = w = 0.0f; }
        Vector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
        //! Contructor with another Vector3 and w.
        Vector4(const Vector3& other, float _w=1.0f) : x(other.x), y(other.y), z(other.z), w(_w) {}
        //! Contructor with another Vector4.
        Vector4(const Vector4& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}
        
        //! Operators
        Vector4 operator-() const { return Vector4(-x, -y, -z, -w); }
        Vector4 &operator=(const Vector4& other) { x = other.x; y = other.y; z = other.z; w = other.w; return *this; }
        Vector4 operator+(const Vector4& other) const { return Vector4(x + other.x, y + other.y, z + other.z, w + other.w); }
        Vector4 operator-(const Vector4& other) const { return Vector4(x - other.x, y - other.y, z - other.z, w - other.w); }
        Vector4 operator*(float value) const { return Vector4(x*value, y*value, z*value, w*value); }
        Vector4 operator/(float value) const { return Vector4(x / value, y / value, z / value, w / value); }
        bool operator==(const Vector4& other) const
        {
            return FLOAT_EQUAL(x, other.x) && FLOAT_EQUAL(y, other.y) && FLOAT_EQUAL(z, other.z) && FLOAT_EQUAL(w, other.w);
        }
        bool operator!=(const Vector4& other) const
        {
            return !FLOAT_EQUAL(x, other.x) || !FLOAT_EQUAL(y, other.y) || !FLOAT_EQUAL(z, other.z) || !FLOAT_EQUAL(w, other.w);
        }
        //! multiply with matrix4
        Vector4 operator*(const Matrix4& other) const;
        
        //! Get vector4 length.
        float length() const { return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2) + pow(w, 2)); }
        //! Normalize vector4.
        void normalize()
        {
            float length = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2) + pow(w, 2));
            if (length > 0.0f) {
                x /= length; y /= length; z /= length; w /= length;
            }
        }
        
    public:
        float x, y, z, w;
    };
    //! Define zero vector4.
    const Vector4 Vector4Zero;
}

#endif // PEACH3D_VECTOR4_H
