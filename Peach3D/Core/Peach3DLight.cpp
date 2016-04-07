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
        // delete a light, node need update valid name
        SceneManager::getSingleton().updateAllNodesLighting();
        // delete shadow texture
        if (mShadowTexture) {
            ResourceManager::getSingleton().deleteTexture(mShadowTexture);
            mShadowTexture = nullptr;
        }
    }
    
    void Light::usingAsDirection(const Vector3& dir, const Color3& color, const Color3& ambient)
    {
        mType = LightType::eDirection; mDir = dir; mColor = color; mAmbient = ambient;
        SceneManager::getSingleton().updateAllNodesLighting();
    }
    
    void Light::usingAsDot(const Vector3& pos, const Vector3& attenuate, const Color3& color, const Color3& ambient)
    {
        mType = LightType::eDot; mPos = pos; mAttenuate = attenuate; mColor = color; mAmbient = ambient;
        SceneManager::getSingleton().updateAllNodesLighting();
    }
    
    void Light::usingAsSpot(const Vector3& pos, const Vector3& dir, const Vector3& attenuate, const Vector2& ext, const Color3& color, const Color3& ambient)
    {
        mType = LightType::eSpot; mPos = pos; mDir = dir; mAttenuate = attenuate; mSpotExt = ext; mColor = color; mAmbient = ambient;
        SceneManager::getSingleton().updateAllNodesLighting();
    }
    
    void Light::setEnabled(bool enable)
    {
        mIsEnabled = enable;
        // SceneNode may need update lighting count
        SceneManager::getSingleton().updateAllNodesLighting();
    }
    
    void Light::setShadowEnabled(bool enable, float factor)
    {
        if (enable && !mShadowTexture) {
            factor = std::max(factor, 1.f);
            auto shadowSize = LayoutManager::getSingleton().getScreenSize() * factor;
            // create shadow depth texture
            mShadowTexture = ResourceManager::getSingleton().createRenderTexture(int(shadowSize.x), int(shadowSize.y), true);
            mShadowTexture->setBeforeRenderingFunc([&]{
                auto smger = SceneManager::getSingletonPtr();
                // save camera state
                auto camera = smger->getActiveCamera();
                mLastCameraState = camera->getState();
                // save projection matrix
                mIsRestoreProj = smger->isOrthoProjection() != (mType == LightType::eDirection);
                if (mIsRestoreProj) {
                    mLastProjMatrix = smger->getProjectionMatrix();
                    if (mType == LightType::eDirection) {
                        smger->setOrthoProjection(-shadowSize.x, shadowSize.x, -shadowSize.y, shadowSize.y, 1.f, 1000.f);
                    }
                    else {
                        smger->setPerspectiveProjection(180, shadowSize.x/shadowSize.y);
                    }
                }
                // move camera to light pos
                if (mType == LightType::eDirection) {
                    camera->setPosition(mShadowFocus - mDir);
                }
                else {
                    camera->setPosition(mPos);
                }
                camera->lockToPosition(mShadowFocus);
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
    }
}

#endif /* PEACH3D_LIGHT_CPP */
