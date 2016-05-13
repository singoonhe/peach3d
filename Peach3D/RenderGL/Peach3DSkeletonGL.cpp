//
//  Peach3DSkeletonGL.cpp
//  Peach3DLib
//
//  Created by singoon.he on 5/11/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "Peach3DSkeletonGL.h"
#include "Peach3DIPlatform.h"

namespace Peach3D
{
    SkeletonGL::~SkeletonGL()
    {
        if (mSTexId) {
            glDeleteTextures(1, &mSTexId);
            mSTexId = 0;
        }
        if (mSTexData) {
            free(mSTexData);
            mSTexData = nullptr;
        }
    }
    
    void SkeletonGL::addBonesOver()
    {
        ISkeleton::addBonesOver();
        auto boneCount = mCacheBones.size();
        if (!mSTexId && boneCount > 0) {
            glGenTextures(1, &mSTexId);
            glBindTexture(GL_TEXTURE_2D, mSTexId);
            if (PD_RENDERLEVEL_GL3()) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 3, mCacheBones.size(), 0, GL_RGBA, GL_FLOAT, 0);
            }
            else {
#if PEACH3D_CURRENT_RENDER == PEACH3D_RENDER_GLES
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 3, mCacheBones.size(), 0, GL_RGBA, GL_FLOAT, 0);
#else
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, 3, mCacheBones.size(), 0, GL_RGBA, GL_FLOAT, 0);
#endif
            }
            mSTexData = (float*)malloc(3 * mCacheBones.size() * 4 * sizeof(float));
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    
    void SkeletonGL::fillAnimateBuffer(const std::string& name, float time)
    {
        ISkeleton::fillAnimateBuffer(name, time);
        
        // fill data to texture
        if (mSTexId && mSTexData) {
            for (auto i=0; i<mCacheBoneMats.size(); ++i) {
                float* sourceData = mCacheBoneMats[i].mat;
                mSTexData[i * 12 + 0] = sourceData[0];
                mSTexData[i * 12 + 1] = sourceData[4];
                mSTexData[i * 12 + 2] = sourceData[8];
                mSTexData[i * 12 + 3] = sourceData[12];
                
                mSTexData[i * 12 + 4] = sourceData[1];
                mSTexData[i * 12 + 5] = sourceData[5];
                mSTexData[i * 12 + 6] = sourceData[9];
                mSTexData[i * 12 + 7] = sourceData[13];
                
                mSTexData[i * 12 + 8] = sourceData[2];
                mSTexData[i * 12 + 9] = sourceData[6];
                mSTexData[i * 12 + 10] = sourceData[10];
                mSTexData[i * 12 + 11] = sourceData[14];
            }
            glBindTexture(GL_TEXTURE_2D, mSTexId);
            if (PD_RENDERLEVEL_GL3()) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 3, mCacheBones.size(), 0, GL_RGBA, GL_FLOAT, mSTexData);
            }
            else {
#if PEACH3D_CURRENT_RENDER == PEACH3D_RENDER_GLES
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 3, mCacheBones.size(), 0, GL_RGBA, GL_FLOAT, mSTexData);
#else
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, 3, mCacheBones.size(), 0, GL_RGBA, GL_FLOAT, mSTexData);
#endif
            }
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
}