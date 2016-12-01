//
//  Peach3DSliderBar.cpp
//  Peach3DLib
//
//  Created by singoon.he on 23/11/2016.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "Peach3DSliderBar.hpp"
#include "Peach3DResourceManager.h"

namespace Peach3D
{
    SliderBar* SliderBar::create(const char* bgImage, const char* barImage, const char* markImage, ProgressBarType type, Vector2 barOffset)
    {
        SliderBar* slider = new SliderBar();
        slider->setBarType(type);
        if (bgImage) {
            auto bgTex = ResourceManager::getSingleton().addTexture(bgImage);
            slider->setTextureFrame(bgTex);
            if (barImage) {
                // create bar sprite
                auto barTex = ResourceManager::getSingleton().addTexture(barImage);
                slider->setBarTexture(barTex);
                if (barOffset == Vector2Zero) {
                    slider->setBarOffset(Vector2((bgTex->getWidth() - barTex->getWidth()) / 2.f, (bgTex->getHeight() - barTex->getHeight()) / 2.f));
                }
                else {
                    slider->setBarOffset(barOffset);
                }
                // create mark sprite
                if (markImage) {
                    auto markTex = ResourceManager::getSingleton().addTexture(markImage);
                    slider->setMarkTexture(markTex);
                }
            }
        }
        return slider;
    }
    
    void SliderBar::setMarkTexture(const TextureFrame& frame)
    {
        if (frame.tex) {
            if (!mMarkSprite) {
                mMarkSprite = Sprite::create(frame);
                addChild(mMarkSprite);
                
                mMarkSprite->setClickEnabled(true);
                mMarkSprite->setClickZoomed(false);
                mMarkSprite->setClickedAction([&](const Vector2& pos) {
                    if (mDownFrame.tex) {
                        mMarkSprite->setTextureFrame(mNormalFrame);
                    }
                }, ClickEvent::eUp);
                mMarkSprite->setClickedAction([&](const Vector2& pos) {
                    if (mDownFrame.tex) {
                        mMarkSprite->setTextureFrame(mNormalFrame);
                    }
                }, ClickEvent::eCancel);
                mMarkSprite->setClickedAction([&](const Vector2& pos) {
                    if (mDownFrame.tex) {
                        mMarkSprite->setTextureFrame(mDownFrame);
                    }
                    mDownPos = pos;
                }, ClickEvent::eDown);
                mMarkSprite->setClickedAction([&](const Vector2& pos) {
                    auto offset = pos - mDownPos;
                    if (mType == ProgressBarType::eHorizontal) {
                        float rangeRate = mCurRate + offset.x / (mBarSize.x - mMarOffset * 2);
                        this->setCurrentProgress(rangeRate);
                    }
                    else if (mType == ProgressBarType::eVertical) {
                        float rangeRate = mCurRate + offset.y / (mBarSize.y - mMarOffset * 2);
                        this->setCurrentProgress(rangeRate);
                    }
                    mDownPos = pos;
                }, ClickEvent::eDrag);
            }
            else {
                mMarkSprite->setTextureFrame(frame);
            }
            mNormalFrame = frame;
            // also will update mark position
            setCurrentProgress(mCurRate);
        }
    }
    
    void SliderBar::updateProgress()
    {
        if (mNeedUpdate && mBarSprite && mMarkSprite) {
            ProgressBar::updateProgress();
            // set mark sprite position
            auto barPos = mBarSprite->getPosition();
            if (mType == ProgressBarType::eHorizontal) {
                float range = mBarSize.x - mMarOffset * 2;
                mMarkSprite->setPosition(barPos + Vector2(range * mCurRate + mMarOffset, mBarSize.y * 0.5f));
            }
            else if (mType == ProgressBarType::eVertical) {
                float range = mBarSize.y - mMarOffset * 2;
                mMarkSprite->setPosition(barPos + Vector2(mBarSize.x * 0.5f, range * mCurRate + mMarOffset));
            }
        }
    }
}
