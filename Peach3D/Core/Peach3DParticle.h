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
        ~Particle() {}
        const std::string& getName() { return mName; }
        
    protected:
        /* Update rendering attributes. */
        virtual void updateRenderingAttributes(float lastFrameTime);
        
    private:
        std::string mName;
        std::vector<Emitter>    mEmitters;
    };
    
    // make shared particle simple
    using ParticlePtr = std::shared_ptr<Particle>;
}

#endif /* PEACH3D_PARTICLE_H */
