//
//  Peach3DSprite.cpp
//  Peach3DLib
//
//  Created by singoon.he on 12/23/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#include "Peach3DSprite.h"
#include "Peach3DUtils.h"
#include "Peach3DResourceManager.h"
#include "Peach3DEventDispatcher.h"

namespace Peach3D
{
    Sprite* Sprite::create()
    {
        Sprite* ns = new Sprite();
        return ns;
    }
    
    Sprite* Sprite::create(const char* texName, const Rect& coord)
    {
        TextureFrame outFrame;
        bool retSuccess = ResourceManager::getSingleton().getTextureFrame(texName, &outFrame);
        if (retSuccess) {
            return Sprite::create(outFrame);
        }
        return nullptr;
    }
    
    Sprite* Sprite::create(const TextureFrame& frame)
    {
        Sprite* ns = nullptr;
        if (frame.tex) {
            ns = new Sprite();
            // add texture, content size also will be changed
            ns->setTextureFrame(frame);
            ns->resetCotentSizeWithTexture();
        }
        return ns;
    }
    
    Sprite::Sprite() : mIsUse9Scale(false), mIsGrayscale(false), mIsAutoResize(true),
        mIsEnabled(false), mIsClickZoomed(true), mCurrentZoomed(false)
    {
        // default to set alpha 1.0f for texture UI
        setAlpha(1.0f);
        setInheritAttri(InheritAttriType::Alpha | InheritAttriType::AutoScale);
        // init child sprites for scale 9 type
        for (auto i=0; i<9; ++i) {
            m9Child[i] = nullptr;
        }
        auto dStep = 1.f / 3.f;
        mCenterRect = Rect(Vector2(dStep), Vector2(dStep));
    }
    
    void Sprite::setTextureFrame(const TextureFrame& frame)
    {
        Peach3DAssert(frame.tex, "Can't set null texture to sprite!");
        if (frame.tex) {
            mRenderFrame = frame;
            mIsRenderHashDirty = true;
            // reset content size, scale 9 or label not need set to texture size
            if (mIsAutoResize) {
                resetCotentSizeWithTexture();
            }
            // update scale9 children texture rect
            setCenterRect(mCenterRect);
        }
    }
    
    void Sprite::resetCotentSizeWithTexture()
    {
        Vector2 texSize((float)mRenderFrame.tex->getWidth(), (float)mRenderFrame.tex->getHeight());
        Widget::setContentSize(texSize * mRenderFrame.rc.size);
    }
    
    void Sprite::setScale9Enabled(bool enable)
    {
        if (mIsUse9Scale == enable)
            return ;
        
        mIsUse9Scale = enable;
        mIsRenderDirty = true;
        if (mIsUse9Scale) {
            // hide self, show children
            setAlpha(0.f);
            setInheritAttri(getInheritAttri() & (0xFFFFFFFF ^ InheritAttriType::Alpha));
            // create scale9 children widgets if enable
            if (!m9Child[0] && mRenderFrame.tex) {
                for (auto i=0; i<9; ++i) {
                    m9Child[i] = Sprite::create(mRenderFrame);
                    addChild(m9Child[i]);
                    // make children zorder low enough
                    m9Child[i]->setLocalZOrder(std::numeric_limits<int>::min() + 100);
                }
                // set default center rect
                setCenterRect(mCenterRect);
            }
        }
        else if (m9Child[0]) {
            for (auto i=0; i<9; ++i) {
                m9Child[i]->deleteFromParent();
                m9Child[i] = nullptr;
            }
            // show self
            setAlpha(1.f);
            setInheritAttri(getInheritAttri() | InheritAttriType::Alpha);
        }
    }
    
    void Sprite::setCenterRect(const Rect& rc)
    {
        Peach3DAssert(rc.pos.x > 0.f && rc.pos.x < 1.f && rc.pos.y > 0.f && rc.pos.y < 1.f && rc.size.x> 0.f && rc.size.x < 1.f && rc.size.y > 0.f && rc.size.y < 1.f && (rc.pos.x + rc.size.x) < 1.f && (rc.pos.y + rc.size.y) < 1.f, "Invalid center rect, every item must be clamp (0-1)");
        mCenterRect = rc;
        if (mIsUse9Scale && m9Child[0]) {
            auto rtSize = mCenterRect.pos + mCenterRect.size;
            float xSizeV[] = {0.f, mCenterRect.pos.x, rtSize.x, 1.f};
            float ySizeV[] = {0.f, mCenterRect.pos.y, rtSize.y, 1.f};
            for (auto i=0; i<9; ++i) {
                auto curFrame = mRenderFrame;
                int xIndex = i % 3;
                int yIndex = int(i / 3);
                auto startPos = Vector2(xSizeV[xIndex], ySizeV[yIndex]);
                curFrame.rc.pos = curFrame.rc.pos + curFrame.rc.size * startPos;
                auto endPos = Vector2(xSizeV[xIndex + 1], ySizeV[yIndex + 1]);
                curFrame.rc.size = curFrame.rc.size * (endPos - startPos);
                m9Child[i]->setTextureFrame(curFrame);
                // set children anchor point to four corners
                m9Child[i]->setAnchorPoint(Vector2(0.f, 0.f));
            }
        }
    }
    
    void Sprite::setClickEnabled(bool enabled)
    {
        // create or delete event listener
        if (!mIsEnabled && enabled) {
            EventDispatcher::getSingletonPtr()->addClickEventListener(this, [&](ClickEvent event, const Vector2& pos) {
                // auto zoom when clicked
                if (mIsClickZoomed) {
                    switch (event) {
                        case ClickEvent::eDown:
                        case ClickEvent::eDragIn:
                            this->setScale(mScale * (mCurrentZoomed ? 1.0f : 1.1f));
                            mCurrentZoomed = true;
                            break;
                        case ClickEvent::eUp:
                        case ClickEvent::eDragOut:
                        case ClickEvent::eCancel:
                            this->setScale(mScale * (mCurrentZoomed ? 0.90909091f : 1.0f));
                            mCurrentZoomed = false;
                            break;
                        default:
                            break;
                    }
                }
                // trigger button registered event
                if (mEventFuncMap.find(event) != mEventFuncMap.end()) {
                    mEventFuncMap[event](pos);
                }
            });
        }
        else if (mIsEnabled && !enabled) {
            EventDispatcher::getSingletonPtr()->deleteClickEventListener(this);
        }
        
        // change button image
        mIsEnabled = enabled;
    }
    
    void Sprite::setClickedAction(ControlListenerFunction func, ClickEvent type)
    {
        mEventFuncMap[type] = func;
        // enable touch if disabled
        setClickEnabled(true);
    }
    
    void Sprite::updateRenderingAttributes(float lastFrameTime)
    {
        if (mIsRenderDirty) {
            // update content size and position ...
            Widget::updateRenderingAttributes(lastFrameTime);
            
            if (mIsUse9Scale && m9Child[0]) {
                auto texWidth = mRenderFrame.tex->getWidth() * mRenderFrame.rc.size.x;
                auto texHeight = mRenderFrame.tex->getHeight() * mRenderFrame.rc.size.y;
                // ratio scale if content size is too small
                if (texWidth > mDesignSize.x || texHeight > mDesignSize.y) {
                    texWidth = mDesignSize.x;
                    texHeight = mDesignSize.y;
                }
                // calc children size
                auto rSizeV = Vector2(1.f) - (mCenterRect.pos + mCenterRect.size);
                float xSizeV[] = {texWidth * mCenterRect.pos.x, 0.f, texWidth * rSizeV.x};
                xSizeV[1] = mDesignSize.x - xSizeV[0] - xSizeV[2];
                float ySizeV[] = {texHeight * mCenterRect.pos.y, 0.f, texHeight * rSizeV.y};
                ySizeV[1] = mDesignSize.y - ySizeV[0] - ySizeV[2];
                // calc children start pos
                float xPosV[] = {0.f, xSizeV[0], mDesignSize.x - xSizeV[2]};
                float yPosV[] = {0.f, ySizeV[0], mDesignSize.y - ySizeV[2]};
                // update all children position and content
                for (auto i=0; i<9; ++i) {
                    int xIndex = i % 3;
                    int yIndex = int(i / 3);
                    m9Child[i]->setPosition(Vector2(xPosV[xIndex], yPosV[yIndex]));
                    m9Child[i]->setContentSize(Vector2(xSizeV[xIndex], ySizeV[yIndex]));
                }
            }
        }
        else {
            // maybe other state need update
            Widget::updateRenderingAttributes(lastFrameTime);
        }
        
        // make widget used RTT active
        if (isNeedRender() && mRenderFrame.tex->getType() == TextureType::eRTT) {
            mRenderFrame.tex->setActived(true);
        }
    }
    
    void Sprite::updateRenderingState(const std::string& extState)
    {
        if (mIsRenderHashDirty) {
            // set default program, then Widget::updateRenderingState will not change it
            if (!mRenderProgram) {
                mRenderProgram = ResourceManager::getSingleton().getPresetProgram({{PROGRAM_FEATURE_POINT3, 0}, {PROGRAM_FEATURE_UV, 1}});
            }
            // sprite will not render if texture is null
            Widget::updateRenderingState(mRenderFrame.tex ? mRenderFrame.tex->getName() : "");
        }
    }
}
