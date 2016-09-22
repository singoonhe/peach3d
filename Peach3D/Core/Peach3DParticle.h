//
//  Peach3DParticle.h
//  Peach3DLib
//
//  Created by singoon.he on 5/15/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#ifndef PEACH3D_PARTICLE_H
#define PEACH3D_PARTICLE_H

#include "Peach3DParticleEmitter.h"
#include "Peach3DWidget.h"
#include "Peach3DSceneNode.h"

namespace Peach3D
{
    class PEACH3D_DLL Particle
    {
    public:
        Particle(const char* name) : mName(name) {}
        virtual ~Particle() {}
        const std::string& getName() { return mName; }
        const std::vector<Emitter>& getEmitters() { return mEmitters; }
        
        void start();
        void end();
        
    protected:
        /* Update rendering attributes. */
        virtual void updateRenderingAttributes(float lastFrameTime);
        
    protected:
        std::string mName;
        std::vector<Emitter>    mEmitters;
    };
    
    /************************************** 2D particle emitter ***************************************/
    class PEACH3D_DLL Particle2D : public Particle, public Widget
    {
    public:
        Particle2D(const char* name) : Particle(name) {}
        ~Particle2D() {}
        
    protected:
        /* Update rendering attributes. */
        virtual void updateRenderingAttributes(float lastFrameTime);
        
    private:
        Vector2     mPos;
        Vector2     mWorldPos;
    };
    
    /************************************** 3D particle emitter ***************************************/
    class PEACH3D_DLL Particle3D : public Particle, public SceneNode
    {
    public:
        Particle3D(const char* name) : Particle(name) {}
        ~Particle3D() {}
        
    protected:
        /* Update rendering attributes. */
        virtual void updateRenderingAttributes(float lastFrameTime);
        
    private:
        Vector3     mPos;
        Vector3     mWorldPos;
    };
}

#endif /* PEACH3D_PARTICLE_H */
