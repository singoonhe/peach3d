//
//  Peach3DRect.h
//  TestPeach3D
//
//  Created by singoon he on 12-7-8.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#ifndef PEACH3D_RECT_H
#define PEACH3D_RECT_H

#include "Peach3DVector2.h"

namespace Peach3D
{
    struct PEACH3D_DLL Rect
    {
    public:
        Rect() {}
        //! Constructor with Vector2 params.
        Rect(const Vector2& _pos, const Vector2& _size):pos(_pos),size(_size) {}
        //! Constructor with four params.
        Rect(float _x, float _y, float _width, float _height)
        {
            pos.x = _x; pos.y = _y;
            size.x = _width; size.y = _height;
        }
        
        //! Operators
        Rect &operator=(const Rect& other)
        {
            pos.x = other.pos.x; pos.y = other.pos.y;
            size.x = other.size.x; size.y = other.size.y;
            return *this;
        }

        //! Test point inside.
        bool pointInside(const Vector2& point)
        {
            float right = pos.x + size.x;
            float bottom = pos.y + size.y;
            return point.x >= pos.x && point.y >= pos.y && point.x < right && point.y < bottom;
        }
        
        //! get intersect with other Rect
        Rect getIntersectRect(const Rect& other)
        {
            Rect rr;
            Vector2 rt = Vector2(pos.x + size.x, pos.y + size.y);
            Vector2 ort = Vector2(other.pos.x + other.size.x, other.pos.y + other.size.y);
            rr.pos.x = (pos.x > other.pos.x) ? pos.x : other.pos.x;
            rr.size.x = ((rt.x < ort.x) ? rt.x : ort.x) - rr.pos.x;
            rr.pos.y = (pos.y > other.pos.y) ? pos.y : other.pos.y;
            rr.size.y = ((rt.y < ort.y) ? rt.y : ort.y) - rr.pos.y;
            return rr;
        }

    public:
        Vector2 pos, size;
    };
}

#endif // PEACH3D_RECT_H
