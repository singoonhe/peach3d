//
//  Peach3DParticle.cpp
//  Peach3DLib
//
//  Created by singoon.he on 5/15/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "Peach3DParticle.h"
#include "Peach3DWidget.h"

namespace Peach3D
{
    void Particle::updateRenderingAttributes(float lastFrameTime)
    {
        Widget* parent = dynamic_cast<Widget*>(mParentNode);
        if (parent) {
            mWorldPos = mPos + parent->getPosition(TranslateRelative::eWorld);
        }
        
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
    
    void Particle::setPosition(const Vector2& pos)
    {
        if (pos != mPos) {
            mPos = pos;
            // set children need update attributes
            setNeedUpdateRenderingAttributes();
        }
    }
    
    const Vector2& Particle::getPosition(TranslateRelative type)
    {
        if (type == TranslateRelative::eWorld) {
            updateRenderingAttributes(0.0f);
            return mWorldPos;
        }
        else {
            return mPos;
        }
    }
}