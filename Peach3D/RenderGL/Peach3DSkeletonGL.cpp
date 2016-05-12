//
//  Peach3DSkeletonGL.cpp
//  Peach3DLib
//
//  Created by singoon.he on 5/11/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "Peach3DSkeletonGL.h"

namespace Peach3D
{
    SkeletonGL::~SkeletonGL()
    {
        if (mBufferId) {
            glDeleteTextures(1, &mBufferId);
            mBufferId = 0;
        }
        if (mSTexId) {
            glDeleteTextures(1, &mSTexId);
            mSTexId = 0;
        }
    }
    
    void SkeletonGL::addBonesOver()
    {
        auto boneCount = mCacheBones.size();
        if (!mBufferId && !mSTexId && boneCount > 0) {
//            glGenBuffers(1, &mBufferId);
//            
//            glBindBuffer(GL_TEXTURE_BUFFER, pModel->JointMatInfo.nBufferObject);
//            
//            glBufferData(GL_TEXTURE_BUFFER, MATRIX4X3ELEMS * nJointCount * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
//            
//            glGenTextures(1, &pModel->JointMatInfo.nTexHandleJointMat);
//            
//            glBindTexture(GL_TEXTURE_BUFFER, pModel->JointMatInfo.nTexHandleJointMat);
//            
//            glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, pModel->JointMatInfo.nBufferObject);  
            
        }
    }
}