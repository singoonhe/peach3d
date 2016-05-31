//
//  Peach3DParticle.cpp
//  Peach3DLib
//
//  Created by singoon.he on 5/15/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "Peach3DParticle.h"
#include "Peach3DWidget.h"
#include "Peach3DSceneNode.h"

namespace Peach3D
{
    void Particle::updateRenderingAttributes(float lastFrameTime)
    {
        for (auto emitter : mEmitters) {
            emitter.update(lastFrameTime);
        }
    }
    
    void Particle::start()
    {
        for (auto emitter : mEmitters) {
            emitter.start();
        }
    }
    
    void Particle::end()
    {
        for (auto emitter : mEmitters) {
            emitter.end();
        }
    }
    
    /************************************** 2D particle emitter ***************************************/
    
    void Particle2D::render()
    {
    }
    
    void Particle2D::setPosition(const Vector2& pos)
    {
        if (pos != mPos) {
            mPos = pos;
            // set children need update attributes
            setNeedUpdateRenderingAttributes();
        }
    }
    
    const Vector2& Particle2D::getPosition(TranslateRelative type)
    {
        if (type == TranslateRelative::eWorld) {
            updateRenderingAttributes(0.0f);
            return mWorldPos;
        }
        else {
            return mPos;
        }
    }
    
    void Particle2D::updateRenderingAttributes(float lastFrameTime)
    {
        Widget* parent = dynamic_cast<Widget*>(mParentNode);
        if (parent) {
            mWorldPos = mPos + parent->getPosition(TranslateRelative::eWorld);
        }
        Particle::updateRenderingAttributes(lastFrameTime);
    }
    
    /************************************** 2D particle emitter ***************************************/
    
    void Particle3D::render()
    {
    }
    
    void Particle3D::setPosition(const Vector3& pos)
    {
        if (pos != mPos) {
            mPos = pos;
            // set children need update attributes
            setNeedUpdateRenderingAttributes();
        }
    }
    
    const Vector3& Particle3D::getPosition(TranslateRelative type)
    {
        if (type == TranslateRelative::eWorld) {
            updateRenderingAttributes(0.0f);
            return mWorldPos;
        }
        else {
            return mPos;
        }
    }
    
    void Particle3D::updateRenderingAttributes(float lastFrameTime)
    {
        SceneNode* parent = dynamic_cast<SceneNode*>(mParentNode);
        if (parent) {
            mWorldPos = mPos + parent->getPosition(TranslateRelative::eWorld);
        }
        Particle::updateRenderingAttributes(lastFrameTime);
    }
}