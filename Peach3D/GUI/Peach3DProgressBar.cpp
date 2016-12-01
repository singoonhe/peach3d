//
//  Peach3DProgressBar.cpp
//  Peach3DLib
//
//  Created by singoon.he on 21/11/2016.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "Peach3DProgressBar.hpp"
#include "Peach3DResourceManager.h"
#include "Peach3DIPlatform.h"

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
    
    void ProgressBar::exit()
    {
        Sprite::exit();
        
        if (mBarScheduler) {
            IPlatform::getSingleton().deleteScheduler(mBarScheduler);
            mBarScheduler = nullptr;
        }
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
    
    void ProgressBar::runProgressAction(float dstRate, float lapTime, uint repeatCount, bool moveFront)
    {
        mDstRate = dstRate;
        mLapTime = lapTime;
        mRepeatCount = repeatCount;
        if (repeatCount > 0) {
            mMoveFront = moveFront;
        }
        else {
            mMoveFront = (mCurRate > dstRate) ? false : true;
        }
        
        // start action scheduler
        if (!mBarScheduler) {
            mBarScheduler = IPlatform::getSingleton().addScheduler([&](float interval){
                if (this->isNeedDelete()) {
                    return ;
                }
                double addRate = interval / mLapTime;
                float newRate = mCurRate + (mMoveFront ? addRate : -addRate);
                if (mRepeatCount == 0 && ((mMoveFront && newRate >= mDstRate) || (!mMoveFront && newRate <= mDstRate))) {
                    // moving to target rate, stop scheduler
                    this->setCurrentProgress(mDstRate);
                    mBarScheduler->pause();
                }
                else {
                    if (mRepeatCount > 0) {
                        if (newRate < 0.f) {
                            newRate += 1.f;
                            mRepeatCount--;
                        }
                        else if (newRate > 1.f) {
                            newRate -= 1.f;
                            mRepeatCount--;
                        }
                    }
                    this->setCurrentProgress(newRate);
                }
            });
        }
        else {
            mBarScheduler->start();
        }
    }
    
    void ProgressBar::setCurrentProgress(float rate)
    {
        CLAMP(rate, 0.f, 1.f);
        mCurRate = rate;
        mNeedUpdate = true;
        // call listener function
        if (mProgressFunc) {
            mProgressFunc(mCurRate);
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
