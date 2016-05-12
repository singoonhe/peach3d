//
//  Peach3DSkeletonGL.h
//  Peach3DLib
//
//  Created by singoon.he on 5/11/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#ifndef PEACH3D_SKELETONGL_H
#define PEACH3D_SKELETONGL_H

#include "Peach3DCommonGL.h"
#include "Peach3DISkeleton.h"

namespace Peach3D
{
    class SkeletonGL : public ISkeleton
    {
    public:
        // constructor and destructor must be public, because shared_ptr need call them
        SkeletonGL(const char* name) : ISkeleton(name), mSTexId(0), mBufferId(0) {}
        virtual ~SkeletonGL();
        
        /** Generate bones texture buffer. */
        virtual void addBonesOver();
        GLuint getBonesTBO() { return mSTexId; }
        
    private:
        GLuint      mSTexId;
        GLuint      mBufferId;
    };
}

#endif /* PEACH3D_SKELETONGL_H */
