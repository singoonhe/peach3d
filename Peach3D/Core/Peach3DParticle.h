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
    /************************************** 2D particle emitter ***************************************/
    class PEACH3D_DLL Particle2D : public Node
    {
    public:
        /* Create 2d particle with file(*.ppt). */
        static Particle2D* create(const char* file);
        
        const std::string& getName() { return mName; }
        const std::vector<Emitter2D>& getEmitters() { return mEmitters; }
        
    protected:
        Particle2D(const char* name) : mName(name) {}
        ~Particle2D() {}
        /* Update rendering attributes. */
        virtual void updateRenderingAttributes(float lastFrameTime);
        
    private:
        std::string mName;
        std::vector<Emitter2D>    mEmitters;
        Vector2     mPos;
        Vector2     mWorldPos;
    };
    
    /************************************** 3D particle emitter ***************************************/
    class PEACH3D_DLL Particle3D : public Node
    {
    public:
        /* Create 3d particle with file(*.ppt). */
        static Particle3D* create(const char* file);
        
        const std::string& getName() { return mName; }
        const std::vector<Emitter3D>& getEmitters() { return mEmitters; }
        
    protected:
        Particle3D(const char* name) : mName(name) {}
        ~Particle3D() {}
        /* Update rendering attributes. */
        virtual void updateRenderingAttributes(float lastFrameTime);
        
    private:
        std::string mName;
        std::vector<Emitter3D>    mEmitters;
        Vector3     mPos;
        Vector3     mWorldPos;
    };
}

#endif /* PEACH3D_PARTICLE_H */
