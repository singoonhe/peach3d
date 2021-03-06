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
#include "Peach3DITexture.h"

namespace Peach3D
{
    struct PEACH3D_DLL ParticlePoint
    {
        ParticlePoint() : size(0.f), lenSize(0.f), time(0.f), lifeTime(0.f) {}
        Color4  color;      // point color and alpha
        Color4  lenColor;   // point end color and alpha
        float   rotate;     // point rotation
        float   lenRotate;  // point end rotation
        float   size;       // point size
        float   lenSize;    // point end size
        float   time;       // point current time
        float   lifeTime;   // point life time
    };
    
    class PEACH3D_DLL Emitter
    {
    public:
        Emitter() : mRunningTime(0.f), mIsRunning(false), mData(nullptr), mDataValidSize(0) {}
        ~Emitter();
        
    public:
        /** Start particle points update. */
        virtual void start();
        /** Update particles count and normal attributes(size, color, time, rotate). */
        void update(float lastFrameTime);
        /** Stop particle points update. */
        virtual void end();
        
        /** Return render data. */
        float* getRenderBuffer()const { return mData; }
        /** Return render data size. */
        int getRenderBufferSize()const { return mDataValidSize; }
        
    protected:
        /** Update each particle point every frame. */
        virtual void updatePointAttributes(ParticlePoint* point, float lastFrameTime);
        /** Generate new particle point if count not max. */
        void generatePaticles(int number);
        /** Generate one particle point and rand attributes. */
        virtual ParticlePoint* generateRandPaticles() = 0;
        /** Reset particle point with rand attributes. */
        virtual void randPaticlePointAttributes(ParticlePoint* point);
        
    public:
        uint    maxCount;       // max particle count, generate count per second
        float   duration;       // particle duration, -1 for loop
        float   lifeTime;       // each particle life time
        float   lifeTimeVariance;   // life time += rand(-variance, variance)
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
        
        TextureFrame    texFrame;   // render frame, include texture and coord
        
    protected:
        std::vector<ParticlePoint*> mPoints;   // particle list, not alway valid
        float   mRunningTime;
        bool    mIsRunning;
        
        float*  mData;
        int     mDataValidSize;
    };
    
    /************************************** 2D particle emitter ***************************************/
    
    struct PEACH3D_DLL ParticlePoint2D : public ParticlePoint
    {
        ParticlePoint2D() : ParticlePoint() {}
        Vector2 pos;    // point render pos
        
        Vector2 dir;    // point moving direction and speed
        Vector2 accelerate; // radial and tangential accelerate
        
        float   angle;      // radius emitter
        float   rotatePerSecond; // radius emitter
        float   radius;     // radius emitter
        float   lenRadius;  // radius emitter
    };
    class PEACH3D_DLL Emitter2D : public Emitter
    {
    public:
        // emitter mode
        enum class Mode {
            eGravity,
            eRadius,
        };
        
    public:
        Emitter2D() : Emitter(), emitterMode(Mode::eGravity) {}

    public:
        /** Return particle2d point stride. */
        static int getPointStride() { return mPointStride; }
        /** Update all particles position. */
        void update(float lastFrameTime, const Vector2& curPos);

    protected:
        /** Update attributes for ParticlePoint2D. */
        virtual void updatePointAttributes(ParticlePoint* point, float lastFrameTime);
        /** Generate one particle point and rand attributes. */
        virtual ParticlePoint* generateRandPaticles();
        /** Reset particle point with rand attributes. */
        virtual void randPaticlePointAttributes(ParticlePoint* point);

    public:
        Mode    emitterMode;

        float   emitAngle;                      // all points emit angle, make points moving direction
        float   emitAngleVariance;
        Vector2 emitPos;                        // relative to the Particle2D position
        Vector2 emitPosVariance;

        // gravity emitter
        float   speed;          // points emit speed
        float   speedVariance;
        Vector2 gravity;            // gravity X and gravity Y
        Vector2 accelerate;         // radial and tangential accelerate
        Vector2 accelerateVariance; // radial and tangential accelerate variance

        // radius emitter
        float   startRadius;
        float   startRadiusVariance;
        float   endRadius;
        float   endRadiusVariance;
        float   rotatePerSecond;
        float   rotatePerSecondVariance;

        static int mPointStride;
    };

    /************************************** 3D particle emitter ***************************************/

    struct PEACH3D_DLL ParticlePoint3D : public ParticlePoint
    {
        ParticlePoint3D() : ParticlePoint() {}
        Vector3 pos;        // point render pos
        Vector3 dir;        // point moving direction and speed
    };
    class PEACH3D_DLL Emitter3D : public Emitter
    {
    public:
        Emitter3D() : Emitter() {}

    public:
        /** Return particle3d point stride. */
        static int getPointStride() { return mPointStride; }
        /** Update all particles position. */
        void update(float lastFrameTime, const Vector3& curPos);

    protected:
        /** Update attributes for ParticlePoint3D. */
        virtual void updatePointAttributes(ParticlePoint* point, float lastFrameTime);
        /** Generate one particle point and rand attributes. */
        virtual ParticlePoint* generateRandPaticles();
        /** Reset particle point with rand attributes. */
        virtual void randPaticlePointAttributes(ParticlePoint* point);

    public:
        Vector3     emitDir;         // all points emit direction
        Vector3     emitDirVariance;
        Vector3     emitPos;         // relative to the Particle3D position
        Vector3     emitPosVariance;

        // gravity emitter
        float   speed;          // points emit speed
        float   speedVariance;
        Vector3 gravity;        // gravity X and gravity Y

        static int mPointStride;
    };
}

#endif /* PEACH3D_PARTICLE_EMITTER_H */
