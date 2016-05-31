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
#include "Peach3DNode.h"

namespace Peach3D
{
    class PEACH3D_DLL Particle : public Node
    {
    public:
        Particle(const char* name) : mName(name) {}
        virtual ~Particle() {}
        const std::string& getName() { return mName; }
        
        void start();
        void end();
        
    protected:
        /* Update rendering attributes. */
        virtual void updateRenderingAttributes(float lastFrameTime);
        
    protected:
        std::string mName;
        std::vector<Emitter>    mEmitters;
    };
    // make shared particle simple
    using ParticlePtr = std::shared_ptr<Particle>;
    
    /************************************** 2D particle emitter ***************************************/
    
    class PEACH3D_DLL Particle2D : public Particle
    {
    public:
        Particle2D(const char* name) : Particle(name) {}
        ~Particle2D() {}
        
        /** Set current relative position. */
        void setPosition(const Vector2& pos);
        /** Get relative position or world position. */
        const Vector2& getPosition(TranslateRelative type = TranslateRelative::eLocal);
        
    protected:
        /* Update rendering attributes. */
        virtual void updateRenderingAttributes(float lastFrameTime);
        
    private:
        Vector2     mPos;
        Vector2     mWorldPos;
    };
}

#endif /* PEACH3D_PARTICLE_H */
