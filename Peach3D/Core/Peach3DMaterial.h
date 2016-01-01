//
//  Peach3DMaterial.h
//  Peach3DLib
//
//  Created by singoon.he on 9/28/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#ifndef PEACH3D_MATERIAL_H
#define PEACH3D_MATERIAL_H

#include "Peach3DCompile.h"
#include "Peach3DVector2.h"
#include "Peach3DColor4.h"
#include "Peach3DITexture.h"

namespace Peach3D
{
    struct PEACH3D_DLL Material
    {
        Material():shininess(0.0f),UVScrollTexIndex(PD_UINT_MAX) {}
        //! Operators
        Material &operator=(const Material& other)
        {
            textureList = other.textureList;
            ambient = other.ambient;
            diffuse = other.diffuse;
            specular = other.specular;
            shininess = other.shininess;
            UVScrollSpeed = other.UVScrollSpeed;
            UVScrollTexIndex = other.UVScrollTexIndex;
            return *this;
        }
        // get texture total count
        uint getTextureCount()const { return (uint)textureList.size(); }
        
        /*
        // set UV scroll speed, this will enable texture auto scroll
        //! the vector must be clamped in (-1~1)
        void setUVScrollSpeed(const Vector2& speed)
        {
            mUVScrollSpeed = speed;
            CLAMP(mUVScrollSpeed.x, -1.0f, 1.0f);
            CLAMP(mUVScrollSpeed.y, -1.0f, 1.0f);
        }
        // update current uv scroll offset and return offset
        Vector2 getUpdateUVScrollOffset(float lastFrameTime)
        {
            if (mUVScrollTexIndex < mTextureList.size())
            {
                // update texture coordinate u and v
                if (!FLOAT_EQUAL_0(mUVScrollSpeed.x) || !FLOAT_EQUAL_0(mUVScrollSpeed.y))
                {
                    mCurUVOffset.x += lastFrameTime * mUVScrollSpeed.x;
                    mCurUVOffset.y += lastFrameTime * mUVScrollSpeed.y;
                    // clamp uv coordinate
                    clampValue0_1(&mCurUVOffset.x);
                    clampValue0_1(&mCurUVOffset.y);
                }
            }
            return mCurUVOffset;
        } */
        
    private:
        //! clamp uv (0-1)
        void clampValue0_1(float* value)
        {
            // clamp value to (0-1)
            if (*value < 0.0f) {
                *value += 1.0f;
            }
            else if (*value > 1.0f) {
                *value -= 1.0f;
            }
        }
        
    public:
        Color4  ambient;
        Color4  diffuse;
        Color4  specular;
        float   shininess;
        Vector2 UVScrollSpeed;     // texture uv scroll speed
        Vector2 CurUVOffset;       // current uv scroll offset
        uint    UVScrollTexIndex;  // default is PD_UINT_MAX, not enable uv scroll
        
        std::vector<ITexture*> textureList;    // used texture list
    };
}

#endif // PEACH3D_MATERIAL_H
