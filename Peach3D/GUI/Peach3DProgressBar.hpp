//
//  Peach3DProgressBar.hpp
//  Peach3DLib
//
//  Created by singoon.he on 21/11/2016.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#ifndef PEACH3D_PROGRESSBAR_H
#define PEACH3D_PROGRESSBAR_H

#include "Peach3DCompile.h"
#include "Peach3DSprite.h"
#include "Peach3DScheduler.h"

namespace Peach3D
{
    // define progress bar type
    enum ProgressBarType
    {
        eHorizontal,
        eVertical,
    };
    // define progress bar cut mode
    enum ProgressBarCutMode
    {
        eRightTop,  // right or top of image will be cut when rate not 1
        eLeftBottom,// left or bottom of image will be cut when rate not 1
    };
    
    class PEACH3D_DLL ProgressBar : public Sprite
    {
    public:
        /** Create progress bar, static method.
         * @params barOffset Bar sprite will be set to center of bg if offset is zero.
         */
        static ProgressBar* create(const char* bgImage, const char* barImage, ProgressBarType type = ProgressBarType::eHorizontal, Vector2 barOffset = Vector2Zero);
        static ProgressBar* create() { return new ProgressBar(); }
        
        void setBarType(ProgressBarType type) { mType = type; mNeedUpdate = true; }
        void setBarCutMode(ProgressBarCutMode mode) { mCutMode = mode; mNeedUpdate = true; }
        /** Set bar new texture, create new bar if no bar sprite. */
        void setBarTexture(const TextureFrame& frame);
        /** Set bar position relative of bg sprite, bar sprite anchor is (0, 0). */
        void setBarOffset(const Vector2& offset) { if (mBarSprite) { mBarSprite->setPosition(offset); } }
        const Vector2& getBarOffset() { return mBarSprite ? mBarSprite->getPosition(): Vector2Zero; }
        
        /** Set progress rate, clamp in (0-1). */
        virtual void setCurrentProgress(float rate);
        float getCurrentProgress() { return mCurRate; }
        /** Function will called when current rate changed. */
        void setProgressFunction(const std::function<void(float rate)>& func) { mProgressFunc = func; }
        
        /** Run progress action. 
         @params dstRate Moving to target rate.
         @params lapTime Progress running base time from 0-1.
         @params repeatCount Repeat count of moving frome 0-1.
         @params moveFront Moving front or back, just valid when repeatCount not 0.
         */
        void runProgressAction(float dstRate, float lapTime, uint repeatCount = 0, bool moveFront = true);
        
    protected:
        ProgressBar() : mBarSprite(nullptr), mType(ProgressBarType::eHorizontal), mCutMode(ProgressBarCutMode::eRightTop), mCurRate(1.f), mNeedUpdate(true), mBarScheduler(nullptr), mProgressFunc(nullptr) {}
        virtual ~ProgressBar() {}
        
        /** Delete scheduler if need. */
        virtual void exit();
        /** Update bar state. */
        virtual void updateProgress();
        /** Update rendering attributes, update progress. */
        virtual void updateRenderingAttributes(float lastFrameTime) { Sprite::updateRenderingAttributes(lastFrameTime); updateProgress(); }
        
    protected:
        ProgressBarType     mType;
        ProgressBarCutMode  mCutMode;
        Scheduler*          mBarScheduler;
        
        float   mCurRate;
        bool    mNeedUpdate;
        Sprite* mBarSprite;
        Rect    mBarCoord;
        Vector2 mBarSize;
        
        float   mDstRate;
        float   mLapTime;
        uint    mRepeatCount;
        bool    mMoveFront;
        
        std::function<void(float rate)> mProgressFunc;
    };
}

#endif /* defined(PEACH3D_PROGRESSBAR_H) */
