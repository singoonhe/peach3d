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
        ParticlePoint() : rotate(0.f), endRotate(0.f), size(0.f), endSize(0.f), time(0.f), lifeTime(0.f) {}
        virtual void copy(const ParticlePoint& other) { color = other.color; endColor = other.endColor; rotate = other.rotate; endRotate = other.endRotate; size = other.size; endSize = other.endSize; time = other.time; lifeTime = other.lifeTime;}
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
        Emitter() : mValidCount(0), mRunningTime(0.f), mIsRunning(false) {}
        ~Emitter() {}
        
    public:
        void start();
        /** Update particles count and normal attributes(size, color, time, rotate). */
        void update(float lastFrameTime);
        void end();
        
    protected:
        /** Update each particle point every frame. */
        virtual void updatePointAttributes(ParticlePoint& point, float lastFrameTime) {};
        /** Generate new particle point if count not max. */
        void generatePaticles(int number);
        /** Generate particle with some rand attribute. */
        virtual ParticlePoint generateRandPaticles();
        
    public:
        uint    maxCount;       // max particle count, generate count per second
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
        
    protected:
        std::vector<ParticlePoint> mPoints; // particle list, not alway valid
        int     mValidCount;    // current valid particle count
        float   mRunningTime;
        bool    mIsRunning;
    };
    
    /************************************** 2D particle emitter ***************************************/
    
    struct PEACH3D_DLL ParticlePoint2D : public ParticlePoint
    {
        ParticlePoint2D() : ParticlePoint() {}
        ParticlePoint2D &operator=(const ParticlePoint2D& other){ pos = other.pos; dir = other.dir; color = other.color; endColor = other.endColor; rotate = other.rotate; endRotate = other.endRotate; size = other.size; endSize = other.endSize; time = other.time; lifeTime = other.lifeTime; return *this; }
        Vector2 pos;    // point render pos
        Vector2 dir;    // point moving direction
    };
    class PEACH3D_DLL Emitter2D : public Emitter
    {
    public:
        Emitter2D() : Emitter() {}
        ~Emitter2D() {}
        
    public:
        /** Update all particles position. */
        virtual void update(float lastFrameTime);
        
    protected:
        /** Update attributes for ParticlePoint2D. */
        virtual void updatePointAttributes(ParticlePoint& point, float lastFrameTime);
        /** Add new attributes for ParticlePoint2D. */
        virtual ParticlePoint generateRandPaticles();
        
    public:
        float   emitAngle;
        float   emitAngleVariance;
        Vector2 emitPos;            // relative to the Particle2D position
        Vector2 emitPosVariance;
        
        // gravity emitter
        float   speed;
        float   speedVariance;
        Vector2 gravity;        // gravity X and gravity Y
        Vector2 accelerate;     // radial and tangential accelerate
        Vector2 accelerateVariance; // radial and tangential accelerate variance
    };
    
    /************************************** 3D particle emitter ***************************************/
}

#endif /* PEACH3D_PARTICLE_EMITTER_H */
