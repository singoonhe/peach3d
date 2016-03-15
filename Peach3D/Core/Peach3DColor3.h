//
//  Peach3DColor3.h
//  Peach3DLib
//
//  Created by singoon.he on 2/22/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#ifndef PEACH3D_COLOR3_H
#define PEACH3D_COLOR3_H

namespace Peach3D
{
    class PEACH3D_DLL Color3
    {
    public:
        Color3(float item = 0.f) { r = g = b = item; }
        //! Constructor with four float params.
        Color3(float _r, float _g, float _b) : r(_r), g(_g), b(_b) {}
        
        //! Operators
        Color3 operator=(const Color3& other) { r = other.r; g = other.g; b = other.b; return *this; }
        Color3 operator+(const Color3& other) const { return Color3(r + other.r, g + other.g, b + other.b); }
        Color3 operator+=(const Color3& other) { r += other.r; g += other.g; b += other.b; return *this; }
        Color3 operator-(const Color3& other) const { return Color3(r - other.r, g - other.g, b - other.b); }
        Color3 operator-=(const Color3& other) { r -= other.r; g -= other.g; b -= other.b; return *this; }
        Color3 operator*(const Color3& other) const { return Color3(r*other.r, g*other.g, b*other.b); }
        Color3 operator*=(const Color3& other) { r *= other.r; g *= other.g; b *= other.b; return *this; }
        bool operator==(const Color3& other) const
        {
            return FLOAT_EQUAL(r, other.r) && FLOAT_EQUAL(g, other.g) && FLOAT_EQUAL(b, other.b);
        }
        bool operator!=(const Color3& other) const
        {
            return !FLOAT_EQUAL(r, other.r) || !FLOAT_EQUAL(g, other.g) || !FLOAT_EQUAL(b, other.b);
        }
        
        //! Get 1-Color3.
        void Invert() { r = 1.0f - r; g = 1.0f - g; b = 1.0f - b;}
        //! Make Color3 standard.
        void standard() { CLAMP(r, 0.0f, 1.0f); CLAMP(g, 0.0f, 1.0f); CLAMP(b, 0.0f, 1.0f); }
        
    public:
        float r, g, b;
    };
    
    //! Define common colors.
    const Color3 Color3Black(0.0f, 0.0f, 0.0f);
    const Color3 Color3Red(1.0f, 0.0f, 0.0f);
    const Color3 Color3Green(0.0f, 1.0f, 0.0f);
    const Color3 Color3Blue(0.0f, 0.0f, 1.0f);
    const Color3 Color3Yellow(1.0f, 1.0f, 0.0f);
    const Color3 Color3Magenta(1.0f, 0.0f, 1.0f);
    const Color3 Color3Cyan(0.0f, 1.0f, 1.0f);
    const Color3 Color3White(1.0f, 1.0f, 1.0f);
    const Color3 Color3Gray(0.5f, 0.5f, 0.5f);
}

#endif // PEACH3D_COLOR3_H
