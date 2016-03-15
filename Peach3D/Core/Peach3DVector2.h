//
//  Peach3DVector2.h
//  TestPeach3D
//
//  Created by singoon he on 12-7-4.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#ifndef PEACH3D_VECTOR2_H
#define PEACH3D_VECTOR2_H

#include "Peach3DCompile.h"

namespace Peach3D
{
    class PEACH3D_DLL Vector2
    {
    public:
        Vector2(float item = 0.f) { x = y = item; }
        Vector2(float _x, float _y) : x(_x), y(_y) {}
        //! Contructor with another Vector2.
        Vector2(const Vector2& other) : x(other.x), y(other.y) {}

        //! Operators
        Vector2 operator-() const { return Vector2(-x, -y); }
        Vector2 &operator=(const Vector2& other){ x = other.x; y = other.y; return *this; }
        Vector2 operator+(const Vector2& other) const { return Vector2(x + other.x, y + other.y); }
        Vector2 operator-(const Vector2& other) const { return Vector2(x - other.x, y - other.y); }
        Vector2 operator*(float value) const { return Vector2(x*value, y*value); }
        Vector2 operator*(const Vector2& other) const { return Vector2(x*other.x, y*other.y); }
        Vector2 operator/(float value) const { return Vector2(x / value, y / value); }
        Vector2 operator/(const Vector2& other) const { return Vector2(x / other.x, y / other.y); }
        bool operator==(const Vector2& other) const { return FLOAT_EQUAL(x, other.x) && FLOAT_EQUAL(y, other.y); }
        bool operator!=(const Vector2& other) const { return !FLOAT_EQUAL(x, other.x) || !FLOAT_EQUAL(y, other.y); }

        //! Calculate distance from other Vector2.
        float distance(const Vector2& other) const { return sqrt(pow(x - other.x, 2) + pow(y - other.y, 2)); }
        //! Get projection to another Vector2.
        Vector2 projective(const Vector2& other) const { return other*(dot(other) / (pow(other.x, 2) + pow(other.y, 2))); }
        //! Get vector2 length.
        float length() const { return sqrt(pow(x, 2) + pow(y, 2)); }
        //! Get dot with another Vector2.
        float dot(const Vector2& other) const { return x*other.x + y*other.y; }
        //! Normalize Vector2.
        void normalize()
        {
            float length = sqrt(pow(x, 2) + pow(y, 2));
            if (length > 0.0f)
            {
                x = x/length; y = y/length;
            }
        }

    public:
        float x, y;
    };
    //! Define zero vector2.
    const Vector2 Vector2Zero;
}

#endif // PEACH3D_VECTOR2_H
