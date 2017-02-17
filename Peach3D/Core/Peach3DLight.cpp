//
//  Peach3DLight.cpp
//  Peach3DLib
//
//  Created by singoon.he on 2/22/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#ifndef PEACH3D_LIGHT_CPP
#define PEACH3D_LIGHT_CPP

#include "Peach3DLight.h"
#include "Peach3DLayoutManager.h"
#include "Peach3DSceneManager.h"
#include "Peach3DResourceManager.h"

namespace Peach3D
{
    CameraState Light::mLastCameraState;
    Matrix4     Light::mLastProjMatrix;
    bool        Light::mIsRestoreProj = false;
    
    Light::~Light()
    {
        // delete shadow texture
        if (mShadowTexture) {
            ResourceManager::getSingleton().deleteTexture(mShadowTexture);
            mShadowTexture = nullptr;
        }
        // SceneManager have update scenenode, so do nothing here
    }
    
    void Light::usingAsDirection(const Vector3& dir, const Color3& color, const Color3& ambient)
    {
        mType = LightType::eDirection; mDir = dir; mColor = color; mAmbient = ambient;
    }
    
    void Light::usingAsDot(const Vector3& pos, const Vector3& attenuate, const Color3& color, const Color3& ambient)
    {
        mType = LightType::eDot; mPos = pos; mAttenuate = attenuate; mColor = color; mAmbient = ambient;
    }
    
    void Light::usingAsSpot(const Vector3& pos, const Vector3& dir, const Vector3& attenuate, const Vector2& ext, const Color3& color, const Color3& ambient)
    {
        mType = LightType::eSpot; mPos = pos; mDir = dir; mAttenuate = attenuate; mSpotExt = ext; mColor = color; mAmbient = ambient;
    }
    
    void Light::setEnabled(bool enable)
    {
        mIsEnabled = enable;
        mIsDirty = true;
    }
    
    void Light::setShadowEnabled(bool enable, float factor)
    {
        if (enable && !mShadowTexture) {
            factor = std::max(factor, 1.f);
            auto shadowSize = LayoutManager::getSingleton().getScreenSize() * factor;
            // create shadow depth texture
            mShadowTexture = ResourceManager::getSingleton().createRenderTexture(int(shadowSize.x), int(shadowSize.y), true);
            mShadowTexture->setBeforeRenderingFunc([&, shadowSize]{
                auto smger = SceneManager::getSingletonPtr();
                // save camera state
                auto camera = smger->getActiveCamera();
                mLastCameraState = camera->getState();
                // save projection matrix
                mIsRestoreProj = smger->isOrthoProjection() != (mType == LightType::eDirection);
                if (mIsRestoreProj) {
                    mLastProjMatrix = smger->getProjectionMatrix();
                    if (mType == LightType::eDirection) {
                        float length = mLastCameraState.pos.length();
                        smger->setOrthoProjection(-length, length, -length, length, 1.f, 1000.f);
                    }
                    else {
                        smger->setPerspectiveProjection(180, shadowSize.x/shadowSize.y);
                    }
                }
                // move camera to light pos
                if (mType == LightType::eDirection) {
                    camera->setPosition(mShadowPos);
                    camera->lockToPosition(mShadowPos + mDir);
                }
                else {
                    camera->setPosition(mPos);
                    camera->lockToPosition(mShadowPos);
                }
                
                // bias matrix convert z-value to UV range(0, 1) from (-1, 1)
                const Matrix4 biasMatrix({0.5f, 0.f, 0.f, 0.f,  0.f, 0.5f, 0.f, 0.f,  0.f, 0.f, 0.5f, 0.f,  0.5f, 0.5f, 0.5f, 1.f});
                // save shadow matrix
                mShadowMatrix = biasMatrix * smger->getProjectionMatrix() * camera->getViewMatrix();
            });
            mShadowTexture->setAfterRenderingFunc([&]{
                auto smger = SceneManager::getSingletonPtr();
                // restore camera
                smger->getActiveCamera()->setState(mLastCameraState);
                // restore projection matrix
                if (mIsRestoreProj) {
                    smger->setProjectionMatrix(mLastProjMatrix);
                }
            });
        }
        else if (!enable && mShadowTexture) {
            ResourceManager::getSingleton().deleteTexture(mShadowTexture);
            mShadowTexture = nullptr;
        }
        mIsDirty = true;
    }
    
    bool Light::isIlluminePos(const Vector3& pos, const std::vector<std::string>& ls)
    {
        // all node need direction light
        if (mType == LightType::eDirection) {
            return true;
        }
        // light can't be ignored
        for (auto& igL : ls) {
            if (igL == mName) {
                return false;
            }
        }
        // calc light accumulate for position, ignore light if too far
        auto lenVector = pos - mPos;
        float calLen = lenVector.length();
        return calLen <= mMaxIllumine;
    }
    
    void Light::prepareForRender()
    {
        if (mIsDirty && mType != LightType::eUnknow) {
            if (mType != LightType::eDirection && mIsEnabled) {
                mMaxIllumine = 0.f;
                float maxDis = 100.f;
                if (mAttenuate.z > 0) {
                    // float accuAtten = attenV.x + attenV.y * calLen + attenV.z * calLen * calLen;
                    float delta = mAttenuate.y * mAttenuate.y - 4 * (mAttenuate.z) * (mAttenuate.x - maxDis);
                    if (delta > 0) {
                        mMaxIllumine = (-mAttenuate.y + sqrt(delta)) / (2 * (mAttenuate.x - maxDis));
                    }
                }
                else if (mAttenuate.y > 0) {
                    // float accuAtten = attenV.x + attenV.y * calLen;
                    mMaxIllumine = (maxDis - mAttenuate.x) / mAttenuate.y;
                }
                else {
                    // float accuAtten = attenV.x;
                    mMaxIllumine = FLT_MAX;
                }
            }
            // sene manager need update light, if some node need lighting now
            SceneManager::getSingleton().setUpdateLighting();
            mIsDirty = false;
        }
    }
}

#endif /* PEACH3D_LIGHT_CPP */
