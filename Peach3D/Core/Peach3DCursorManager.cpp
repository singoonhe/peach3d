//
//  Peach3DCursorManager.cpp
//  Peach3DLib
//
//  Created by singoon.he on 1/29/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "Peach3DCursorManager.h"
#include "Peach3DSceneManager.h"

namespace Peach3D
{
    IMPLEMENT_SINGLETON_STATIC(CursorManager);
    
    CursorManager::CursorManager() : mIsCursorEnabled(false), mCursorSprite(nullptr), mCurStatus(0)
    {
        // init texture list
        memset(mStatusTexs, 0, sizeof(mStatusTexs));
        for (auto i = 0; i < CursorState::Count; ++i) {
            mStatusTexs[i] = nullptr;
            mStatusTexRects[i].pos = Vector2Zero;
            mStatusTexRects[i].size = Vector2(1.0f, 1.0f);
        }
    }
    
    void CursorManager::prepareForRender()
    {
        if (mIsCursorEnabled && !mCursorSprite) {
            auto curIndex = getStatusIndex(mCurStatus);
            // create new cursor if not exist
            if (mStatusTexs[curIndex]) {
                mCursorSprite = Sprite::create(mStatusTexs[curIndex], mStatusTexRects[curIndex]);
                mCursorSprite->setLocalZOrder(NOT_CLEAR_WIDGET_ZORDER + 1);
                SceneManager::getSingleton().getRootWidget()->addChild(mCursorSprite);
            }
        }
        else if (!mIsCursorEnabled && mCursorSprite) {
            // cursor no longer exist, delete sprite
            mCursorSprite->deleteFromParent();
            mCursorSprite = nullptr;
        }
    }
    
    void CursorManager::updateCursorPosition(const Vector2& pos)
    {
        if (mCursorSprite) {
            mCursorSprite->setPosition(pos);
        }
    }
    
    void CursorManager::setTextureForStatus(ITexture* tex, uint status)
    {
        uint statusList[] = {CursorState::Normal, CursorState::Down};
        for (uint perStatus : statusList) {
            uint index = getStatusIndex(perStatus);
            if (perStatus & status) {
                mStatusTexs[index] = tex;
            }
        }
        /*
        if (!mCurStatus) {
            // deafult show normal status
            setButtonShowStatus(ButtonState::Normal);
        }
        else if (status == mCurStatus) {
            // update current texture
            setTexture(mStatusTexs[getStatusIndex(mCurStatus)]);
        }
        
        // disable zoomed when Down image set
        if ((status & ButtonState::Down) && tex) {
            setClickZoomed(false);
        }*/
    }
    
    void CursorManager::setTextureRectForStatus(Rect rect, uint status)
    {
        uint statusList[] = {CursorState::Normal, CursorState::Down};
        for (uint perStatus : statusList) {
            uint index = getStatusIndex(perStatus);
            if (perStatus & status) {
                mStatusTexRects[index] = rect;
            }
        }
        
        if (mCurStatus && mCursorSprite) {
            // update current texture rect
            mCursorSprite->setTextureRect(mStatusTexRects[getStatusIndex(mCurStatus)]);
        }
    }
    
    void CursorManager::setButtonShowStatus(uint status)
    {
        if (status != mCurStatus) {
            mCurStatus = status;
            /*
             uint index = getStatusIndex(status);
            if (mCurStatus == CursorState::Disable && !mStatusTexs[index]) {
                setGrayscaleEnabled(true);
            }
            else {
                setGrayscaleEnabled(false);
                // default use normal texture
                index = (!mStatusTexs[index]) ? getStatusIndex(ButtonState::Normal) : index;
                // reset show texture and rect
                if (mStatusTexs[index]) {
                    setTexture(mStatusTexs[index]);
                }
                setTextureRect(mStatusTexRects[index]);
            } */
        }
    }
    
    uint CursorManager::getStatusIndex(uint status)
    {
        uint index = 0;
        if (status & CursorState::Normal) {
            index = 0;
        }
        else if (status & CursorState::Down) {
            index = 1;
        }
        return index;
    }
}