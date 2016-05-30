//
//  Peach3DColor4.h
//  TestPeach3D
//
//  Created by singoon he on 12-7-8.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#ifndef PEACH3D_COLOR4_H
#define PEACH3D_COLOR4_H

#include "Peach3DColor3.h"

namespace Peach3D
{
    class PEACH3D_DLL Color4
    {
    public:
        Color4(float item = 0.f) { r = g = b = a = item; }
        //! Constructor with four float params.
        Color4(float _r, float _g, float _b, float _a = 1.0f) : r(_r), g(_g), b(_b), a(_a) {}
        //! Constructor with Color3 params.
        Color4(const Color3& c) { r = c.r; g = c.g; b = c.b; a = 1.f; }

        //! Operators
        Color4 operator=(const Color4& other) { r = other.r; g = other.g; b = other.b; a = other.a; return *this; }
        Color4 operator+(const Color4& other) const { return Color4(r + other.r, g + other.g, b + other.b, a + other.a); }
        Color4 operator+=(const Color4& other) { r += other.r; g += other.g; b += other.b; a += other.a; return *this; }
        Color4 operator-(const Color4& other) const { return Color4(r - other.r, g - other.g, b - other.b, a - other.a); }
        Color4 operator-=(const Color4& other) { r -= other.r; g -= other.g; b -= other.b; a -= other.a; return *this; }
        Color4 operator*(const Color4& other) const { return Color4(r*other.r, g*other.g, b*other.b, a*other.a); }
        Color4 operator*=(const Color4& other) { r *= other.r; g *= other.g; b *= other.b; a *= other.a; return *this; }
        Color4 operator/(float value) const { return Color4(r/value, g/value, b/value, a/value); }
        bool operator==(const Color4& other) const
        {
            return FLOAT_EQUAL(r, other.r) && FLOAT_EQUAL(g, other.g) && FLOAT_EQUAL(b, other.b) && FLOAT_EQUAL(a, other.a);
        }
        bool operator!=(const Color4& other) const
        {
            return !FLOAT_EQUAL(r, other.r) || !FLOAT_EQUAL(g, other.g) || !FLOAT_EQUAL(b, other.b) || !FLOAT_EQUAL(a, other.a);
        }

        /** Get 1-Color4. */
        void Invert(bool alpha = false) { r = 1.0f - r; g = 1.0f - g; b = 1.0f - b; if (alpha) { a = 1.0f - a; } }
        /** Make Color4 standard. */
        void standard() { CLAMP(r, 0.0f, 1.0f); CLAMP(g, 0.0f, 1.0f); CLAMP(b, 0.0f, 1.0f); CLAMP(a, 0.0f, 1.0f); }

    public:
        float r, g, b, a;
    };
    
    //! Define common colors.
    const Color4 Color4Black(0.0f, 0.0f, 0.0f);
    const Color4 Color4Red(1.0f, 0.0f, 0.0f);
    const Color4 Color4Green(0.0f, 1.0f, 0.0f);
    const Color4 Color4Blue(0.0f, 0.0f, 1.0f);
    const Color4 Color4Yellow(1.0f, 1.0f, 0.0f);
    const Color4 Color4Magenta(1.0f, 0.0f, 1.0f);
    const Color4 Color4Cyan(0.0f, 1.0f, 1.0f);
    const Color4 Color4White(1.0f, 1.0f, 1.0f);
    const Color4 Color4Gray(0.5f, 0.5f, 0.5f);
}

#endif // PEACH3D_COLOR4_H
