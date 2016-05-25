//
//  Peach3DParticleEmitter.h
//  Peach3DLib
//
//  Created by singoon.he on 5/25/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#ifndef PEACH3D_PARTICLE_EMITTER_H
#define PEACH3D_PARTICLE_EMITTER_H

#include "Peach3DCompile.h"
#include "Peach3DVector2.h"
#include "Peach3DVector3.h"
#include "Peach3DColor4.h"

namespace Peach3D
{
    struct PEACH3D_DLL ParticlePoint
    {
        Vector3 pos;    // point render pos
        Color4  colour; // point color and alpha
        float   size;   // point size
        bool    valid;  // is point valid
    };
    
    class PEACH3D_DLL Emitter
    {
    public:
        Emitter() {}
        ~Emitter() {}
        
    public:
        uint    maxCount;
        float   duration;       // particle duration, -1 for loop
        float   lifeTime;       // each particle life time
        float   lifeVariance;   // life time += rand(-variance, variance)
        // particle size
        float   startSize;
        float   startSizeVariance;
        float   endSize;
        float   endSizeVariance;
        // particle rotate
        float   startRotate;
        float   startRotateVariance;
        float   endRotate;
        float   endRotateVariance;
        // particle color
        Color4  startColor;
        Color4  startColorVariance;
        Color4  endColor;
        Color4  endColorVariance;
        float   emitAngle;
        float   emitAngleVariance;
        Vector2 emitPos;
        Vector2 emitPosVariance;
        
        // gravity emitter
        float   speed;
        float   speedVariance;
        Vector2 gravity;        // gravity X and gravity Y
        Vector2 accelerate;     // radial and tangential accelerate
        Vector2 accelerateVariance; // radial and tangential accelerate variance
        
    private:
        std::vector<ParticlePoint> mPoints;
    };
}

#endif /* PEACH3D_PARTICLE_EMITTER_H */
