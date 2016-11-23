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
        
        /** Set mark new texture, create new mark if no mark sprite. */
        void setMarkTexture(const TextureFrame& frame);
        /** Set mark down and drag texture. */
        void setMarkDownTexture(const TextureFrame& frame) { mDownFrame = frame; }
        /** Set mark start and end offset, range = bar_size - 2 * mark_offset. */
        void setMarkOffset(float offset) { mMarOffset = offset; }
        
    public:
        SliderBar() : ProgressBar(), mMarkSprite(nullptr), mMarOffset(0.f) {}
        virtual ~SliderBar() {}
        
        /** Update mark sprite position. */
        virtual void updateProgress();
        
    protected:
        Sprite* mMarkSprite;
        float   mMarOffset;
        Vector2 mDownPos;
        
        TextureFrame    mNormalFrame;
        TextureFrame    mDownFrame;
    };
}

#endif /* defined(PEACH3D_SLIDERBAR_H) */
