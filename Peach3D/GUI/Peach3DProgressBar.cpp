//
//  Peach3DProgressBar.cpp
//  Peach3DLib
//
//  Created by singoon.he on 21/11/2016.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "Peach3DProgressBar.hpp"
#include "Peach3DResourceManager.h"

namespace Peach3D
{
    ProgressBar* ProgressBar::create(const char* bgImage, const char* barImage, ProgressBarType type, Vector2 barOffset)
    {
        ProgressBar* progress = new ProgressBar();
        progress->setBarType(type);
        if (bgImage) {
            auto bgTex = ResourceManager::getSingleton().addTexture(bgImage);
            progress->setTextureFrame(bgTex);
            if (barImage) {
                // create bar sprite
                auto barTex = ResourceManager::getSingleton().addTexture(barImage);
                progress->setBarTexture(barTex);
                if (barOffset == Vector2Zero) {
                    progress->setBarOffset(Vector2((bgTex->getWidth() - barTex->getWidth()) / 2.f, (bgTex->getHeight() - barTex->getHeight()) / 2.f));
                }
                else {
                    progress->setBarOffset(barOffset);
                }
            }
        }
        return progress;
    }
    
    ProgressBar* ProgressBar::create()
    {
        ProgressBar* progress = new ProgressBar();
        return progress;
    }
    
    void ProgressBar::setBarTexture(const TextureFrame& frame)
    {
        if (frame.tex) {
            if (!mBarSprite) {
                mBarSprite = Sprite::create(frame);
                mBarSprite->setAnchorPoint(Vector2Zero);
                addChild(mBarSprite);
            }
            else {
                mBarSprite->setTextureFrame(frame);
            }
            // save bar orgin texture coord and size
            mBarCoord = frame.rc;
            mBarSize = mBarSprite->getContentSize();
        }
    }
    
    void ProgressBar::updateProgress()
    {
        if (mNeedUpdate && mBarSprite) {
            Vector2 size = mBarSize, rcs = mBarCoord.size, rcp = mBarCoord.pos;
            if (mType == ProgressBarType::eHorizontal) {
                size.x *= mCurRate;
                // cut x start part of image
                if (mCutMode == ProgressBarCutMode::eLeftBottom) {
                    rcp.x += rcs.x * (1.f - mCurRate);
                }
                rcs.x *= mCurRate;
            }
            else if (mType == ProgressBarType::eVertical) {
                size.y *= mCurRate;
                // cut y start part of image
                if (mCutMode == ProgressBarCutMode::eLeftBottom) {
                    rcp.y += rcs.y * (1.f - mCurRate);
                }
                rcs.y *= mCurRate;
            }
            mBarSprite->setContentSize(size);
            // also set texture rect
            mBarSprite->setTextureRect(Rect(rcp, rcs));
            
            mNeedUpdate = false;
        }
    }
}
