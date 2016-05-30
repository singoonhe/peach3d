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
    struct PEACH3D_DLL ParticlePoint2
    {
        ParticlePoint2() : rotate(0.f), endRotate(0.f), size(0.f), endSize(0.f), time(0.f), lifeTime(0.f) {}
        ParticlePoint2 &operator=(const ParticlePoint2& other){ pos = other.pos; dir = other.dir; color = other.color; endColor = other.endColor; rotate = other.rotate; endRotate = other.endRotate; size = other.size; endSize = other.endSize; time = other.time; lifeTime = other.lifeTime; return *this; }
        Vector2 pos;        // point render pos
        Vector2 dir;        // point moving direction
        Color4  color;      // point color and alpha
        Color4  endColor;   // point end color and alpha
        float   rotate;     // point rotation
        float   endRotate;  // point end rotation
        float   size;       // point size
        float   endSize;    // point end size
        float   time;       // point current time
        float   lifeTime;   // point life time
    };
    
    class PEACH3D_DLL Emitter
    {
    public:
        Emitter() : mValidCount(0), mRunningTime(0.f), isRunning(false) {}
        ~Emitter() {}
        
    public:
        void start();
        void update(float lastFrameTime);
        void end();
        
    protected:
        void generatePaticles(int number);
        ParticlePoint2 generateRandPaticles();
        
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
        
    protected:
        std::vector<ParticlePoint2> mPoints;
        int     mValidCount;
        float   mRunningTime;
        bool    isRunning;
    };
}

#endif /* PEACH3D_PARTICLE_EMITTER_H */
