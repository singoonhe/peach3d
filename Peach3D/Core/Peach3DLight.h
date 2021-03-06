//
//  Peach3DLight.h
//  Peach3DLib
//
//  Created by singoon.he on 2/22/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#ifndef PEACH3D_LIGHT_H
#define PEACH3D_LIGHT_H

#include "Peach3DCompile.h"
#include "Peach3DColor4.h"
#include "Peach3DVector3.h"
#include "Peach3DMatrix4.h"
#include "Peach3DTypes.h"
#include "Peach3DITexture.h"

namespace Peach3D
{
    enum class PEACH3D_DLL LightType
    {
        eUnknow,    // used for initialize light
        eDirection, // direction light
        eDot,       // dot light
        eSpot,      // spot light
    };
    
    class PEACH3D_DLL Light
    {
    public:
        Light(const char* name) : mName(name), mType(LightType::eUnknow), mIsEnabled(true), mIsDirty(true), mMaxIllumine(0) {}
        ~Light();
        
        /** Direction light only need direction. */
        void usingAsDirection(const Vector3& dir, const Color3& color = Color3Gray);
        /** Dot light need light position and attenuate(const/line/square). */
        void usingAsDot(const Vector3& pos, const Vector3& attenuate = Vector3(1.f, 0.1f, 0.f), const Color3& color = Color3White);
        /** Spot light need light position and direction, add spot angle cos and transverse attenuate(pow) based on the Dot. */
        void usingAsSpot(const Vector3& pos, const Vector3& dir, const Vector3& attenuate = Vector3(1.f, 0.1f, 0.f), const Vector2& ext = Vector2(0.5f, 2.f), const Color3& color = Color3White);
        
        /** Create shadow RTT with texture size, or disable shadow.
         * @params factor must not samller than 1.f, TextureSize = ScreenSize * factor.
         */
        void setShadowEnabled(bool enable, float factor = 1.f);
        /** Return RTT texture or judge is shadow enabled. */
        TexturePtr getShadowTexture() { return mShadowTexture; }
        /** Set shadow locked pos for dot light or camera pos for direction light. */
        void setShadowPos(const Vector3& pos) { mShadowPos = pos; }
        const Vector3& getShadowPos() { return mShadowPos; }
        /** Shadow matrix saved in shadow RTT pass, used in main pass. */
        const Matrix4& getShadowMatrix() { return mShadowMatrix; }
        /** Is position will be illumined by light. */
        bool isIlluminePos(const Vector3& pos, const std::vector<std::string>& ls);
        
        const std::string& getName() { return mName; }
        LightType getType() { return mType; }
        void setEnabled(bool enable);
        bool isEnabled() { return mIsEnabled; }
        void setPosition(const Vector3& pos) { mPos = pos; mIsDirty = true; };
        const Vector3& getPosition() { return mPos; }
        void setDirection(const Vector3& dir) { mDir = dir; };
        const Vector3& getDirection() { return mDir; }
        void setColor(const Color3& color) { mColor = color; };
        const Color3& getColor() { return mColor; }
        void setAttenuate(const Vector3& atten) { mDir = atten; mIsDirty = true; };
        const Vector3& getAttenuate() { return mAttenuate; }
        void setSpotExtend(const Vector2& spotExt) { mSpotExt = spotExt; };
        const Vector2& getSpotExtend() { return mSpotExt; }
        
        /** Update max illumine distance. */
        void prepareForRender();
        
    private:
        std::string mName;       // light name
        bool        mIsEnabled;  // is lighting enable
        LightType   mType;       // light type
        Vector3     mPos;        // light position, for Dot and Spot
        Vector3     mDir;        // light direction, for Direction and Spot
        Color3      mColor;      // light color
        Vector3     mAttenuate;  // light const/line/quadratic attenuate, for Dot and Spot
        Vector2     mSpotExt;    // extent spot and cut cos attenuate, for Spot
        bool        mIsDirty;    // is lighting need update
        float       mMaxIllumine;// max distance light illumine
        
        Matrix4     mShadowMatrix;  // shadow bias matrix
        TexturePtr  mShadowTexture; // world shadow texture
        Vector3     mShadowPos;     // locked pos or pos for shadow camera
        static CameraState mLastCameraState; // cache camera state
        static Matrix4     mLastProjMatrix;  // cache projective matrix
        static bool        mIsRestoreProj;   // is projection matrix need restore
    };
    
    // make shared light simple
    using LightPtr = std::shared_ptr<Light>;
}

#endif /* PEACH3D_LIGHT_H */
