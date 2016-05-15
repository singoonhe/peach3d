//
//  Peach3DParticle.h
//  Peach3DLib
//
//  Created by singoon.he on 5/15/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#ifndef PEACH3D_PARTICLE_H
#define PEACH3D_PARTICLE_H

#include "Peach3DCompile.h"
#include "Peach3DVector2.h"
#include "Peach3DVector3.h"

namespace Peach3D
{
    class Particle
    {
    public:
        Particle(const char* name) : mName(name) {}
        ~Particle() {}
        const std::string& getName() { return mName; }
        
    private:
        std::string mName;
    };
    
    // make shared particle simple
    using ParticlePtr = std::shared_ptr<Particle>;
}

#endif /* PEACH3D_PARTICLE_H */
