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
#include "Peach3DLogPrinter.h"
#include "Peach3DITexture.h"

namespace Peach3D
{
    class PEACH3D_DLL Material
    {
    public:
        Material():mShininess(0.0f),mUVScrollTexIndex(PD_UINT_MAX) {}
        //! Operators
        Material &operator=(const Material& other)
        {
            mTextureList = other.mTextureList;
            mAmbient = other.mAmbient;
            mDiffuse = other.mDiffuse;
            mSpecular = other.mSpecular;
            mShininess = other.mShininess;
            mUVScrollSpeed = other.mUVScrollSpeed;
            mUVScrollTexIndex = other.mUVScrollTexIndex;
            return *this;
        }
        // get texure by index
        ITexture* getTextureByIndex(int index) { if (index < (int)mTextureList.size()) return mTextureList[index]; return nullptr; };
        // get texture total count
        uint getTextureCount()const { return (uint)mTextureList.size(); }
        //! traverse textures, will auto call lambda func
        void tranverseTextures(std::function<void(int, ITexture*)> callFunc)
        {
            for (int i=0; i<(int)mTextureList.size(); ++i)
            {
                // tranverse all texture with param func
                callFunc(i, mTextureList[i]);
            }
        }
        
        // set ambient color
        void setAmbient(const Color4& ambient) { mAmbient = ambient; }
        // get ambient color
        const Color4& getAmbient()const { return mAmbient; }
        
        // set diffuse color
        void setDiffuse(const Color4& diffuse) { mDiffuse = diffuse; }
        // get diffuse color
        const Color4& getDiffuse()const { return mDiffuse; }
        
        // set specular
        void setSpecular(const Color4& specular) { mSpecular = specular; }
        // get specular color
        const Color4& getSpecular()const { return mSpecular; }
        
        // set shininess
        void setShininess(float shininess) { mShininess = shininess; }
        // get shininess float
        float getShininess() { return mShininess; }
        
        // set UV scroll speed, this will enable texture auto scroll
        //! the vector must be clamped in (-1~1)
        void setUVScrollSpeed(const Vector2& speed)
        {
            mUVScrollSpeed = speed;
            CLAMP(mUVScrollSpeed.x, -1.0f, 1.0f);
            CLAMP(mUVScrollSpeed.y, -1.0f, 1.0f);
        }
        // get coordinates UV scroll speed
        Vector2 getUVScrollSpeed()const {return mUVScrollSpeed;}
        // get uv scroll texture index affected
        uint getUVScrollTexIndex()const {return mUVScrollTexIndex;}
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
        }
        
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
        
    private:
        Color4  mAmbient;
        Color4  mDiffuse;
        Color4  mSpecular;
        float   mShininess;
        Vector2 mUVScrollSpeed;     // texture uv scroll speed
        Vector2 mCurUVOffset;       // current uv scroll offset
        uint    mUVScrollTexIndex;  // default is PD_UINT_MAX, not enable uv scroll
        
        std::vector<ITexture*> mTextureList;
        friend class IObject;
        friend class SceneNode;
    };
}

#endif // PEACH3D_MATERIAL_H
