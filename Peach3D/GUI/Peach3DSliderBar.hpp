//
//  Peach3DSliderBar.hpp
//  Peach3DLib
//
//  Created by singoon.he on 23/11/2016.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#ifndef PEACH3D_SLIDERBAR_H
#define PEACH3D_SLIDERBAR_H

#include "Peach3DCompile.h"
#include "Peach3DProgressBar.hpp"

namespace Peach3D
{
    class PEACH3D_DLL SliderBar : public ProgressBar
    {
    public:
        /** Create progress bar, static method.
         * @params barOffset Bar sprite will be set to center of bg if offset is zero.
         */
        static SliderBar* create(const char* bgImage, const char* barImage, const char* markImage, ProgressBarType type = ProgressBarType::eHorizontal, Vector2 barOffset = Vector2Zero);
        static SliderBar* create() { return new SliderBar(); }
        
        /** Set progress rate, clamp in (0-1). */
        virtual void setCurrentProgress(float rate) { CLAMP(rate, 0.f, 1.f); mCurRate = rate; mNeedUpdate = true; }
        
        /** Run progress action.
         @params dstRate Moving to target rate.
         @params lapTime Progress running base time from 0-1.
         @params repeatCount Repeat count of moving frome 0-1.
         @params moveFront Moving front or back, just valid when repeatCount not 0.
         */
//        void runProgressAction(float dstRate, float lapTime, uint repeatCount = 0, bool moveFront = true);
        
    public:
        SliderBar() : ProgressBar(), mMarkSprite(nullptr) {}
        virtual ~SliderBar() {}
        
    protected:
        Sprite* mMarkSprite;
    };
}

#endif /* defined(PEACH3D_SLIDERBAR_H) */
