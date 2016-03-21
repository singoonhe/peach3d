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
#include "Peach3DSceneManager.h"

namespace Peach3D
{
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
}

#endif /* PEACH3D_LIGHT_CPP */
