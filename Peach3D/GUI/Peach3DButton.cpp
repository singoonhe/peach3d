//
//  Peach3DButton.cpp
//  Peach3DLib
//
//  Created by singoon.he on 9/4/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#include "Peach3DButton.h"
#include "Peach3DIPlatform.h"
#include "Peach3DResourceManager.h"
#include "Peach3DCursorManager.h"

namespace Peach3D
{
    Button* Button::create(const char* normal, const char* down, const char* highlight, const char* disable)
    {
        Button* newBtn = new Button();
        TextureFrame outFrame;
        bool isSuccess = ResourceManager::getSingleton().getTextureFrame(normal, &outFrame);
        if (isSuccess) {
            newBtn->setTextureForStatus(outFrame.tex, ButtonState::Normal);
            newBtn->setTextureRectForStatus(outFrame.rc, ButtonState::Normal);
            
            // load down frame
            isSuccess = ResourceManager::getSingleton().getTextureFrame(down, &outFrame);
            if (isSuccess) {
                newBtn->setTextureForStatus(outFrame.tex, ButtonState::Down);
                newBtn->setTextureRectForStatus(outFrame.rc, ButtonState::Down);
                newBtn->setClickZoomed(false);
            }
            // load highlight frame
            isSuccess = ResourceManager::getSingleton().getTextureFrame(highlight, &outFrame);
            if (isSuccess) {
                newBtn->setTextureForStatus(outFrame.tex, ButtonState::Highlight);
                newBtn->setTextureRectForStatus(outFrame.rc, ButtonState::Highlight);
            }
            // load disable frame
            isSuccess = ResourceManager::getSingleton().getTextureFrame(disable, &outFrame);
            if (isSuccess) {
                newBtn->setTextureForStatus(outFrame.tex, ButtonState::Disable);
                newBtn->setTextureRectForStatus(outFrame.rc, ButtonState::Disable);
            }
        }
        return newBtn;
    }
    
    Button* Button::create()
    {
        Button* newBtn = new Button();
        return newBtn;
    }
    
    Button::Button() : mCurStatus(0), mTitleLabel(nullptr), mTitleOffset(0.5f, 0.5f)
    {
        // init texture list
        memset(mStatusTexs, 0, sizeof(mStatusTexs));
        for (auto i = 0; i < ButtonState::Count; ++i) {
            mStatusTexs[i] = nullptr;
            mStatusTexRects[i].pos = Vector2Zero;
            mStatusTexRects[i].size = Vector2(1.0f, 1.0f);
        }
        
        // default to enable button
        setClickEnabled(true);
    }
    
    void Button::setTextureForStatus(ITexture* tex, uint status)
    {
        uint statusList[] = {ButtonState::Normal, ButtonState::Highlight, ButtonState::Down, ButtonState::Disable};
        for (uint perStatus : statusList) {
            uint index = getStatusIndex(perStatus);
            if (perStatus & status) {
                mStatusTexs[index] = tex;
            }
        }
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
        }
    }
    
    void Button::setTextureRectForStatus(Rect rect, uint status)
    {
        uint statusList[] = {ButtonState::Normal, ButtonState::Highlight, ButtonState::Down, ButtonState::Disable};
        for (uint perStatus : statusList) {
            uint index = getStatusIndex(perStatus);
            if (perStatus & status) {
                mStatusTexRects[index] = rect;
            }
        }
        
        if (mCurStatus) {
            // update current texture rect
            setTextureRect(mStatusTexRects[getStatusIndex(mCurStatus)]);
        }
    }
    
    void Button::setButtonShowStatus(uint status)
    {
        if (status != mCurStatus) {
            mCurStatus = status;
            uint index = getStatusIndex(status);
            if (mCurStatus == ButtonState::Disable && !mStatusTexs[index]) {
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
            }
        }
    }
    
    void Button::setContentSize(const Vector2& size)
    {
        Sprite::setContentSize(size);
        // also reset title label position
        setTitleOffset(mTitleOffset);
    }
    
    void Button::setTitleText(const std::string& title)
    {
        if (title.size() > 0 && !mTitleLabel) {
            mTitleLabel = Label::create(title);
            this->addChild(mTitleLabel);
            // init title position
            mTitleLabel->setPosition(mTitleOffset * mRect.size);
        }
        else if (mTitleLabel) {
            mTitleLabel->setText(title);
        }
    }
    
    void Button::setTitleFontName(const std::string& name)
    {
        if (mTitleLabel) {
            mTitleLabel->setFontName(name);
        }
    }
    
    void Button::setTitleFontSize(float size)
    {
        if (mTitleLabel) {
            mTitleLabel->setFontSize(size);
        }
    }
    
    void Button::setTitleColor(const Color4& color)
    {
        if (mTitleLabel) {
            mTitleLabel->setFillColor(color);
        }
    }
    
    void Button::setTitleOffset(const Vector2& offset)
    {
        mTitleOffset = offset;
        if (mTitleLabel) {
            mTitleLabel->setPosition(mTitleOffset * mRect.size);
        }
    }
    
    uint Button::getStatusIndex(uint status)
    {
        uint index = 0;
        if (status & ButtonState::Normal) {
            index = 0;
        }
        else if (status & ButtonState::Highlight) {
            index = 1;
        }
        else if (status & ButtonState::Down) {
            index = 2;
        }
        else if (status & ButtonState::Disable) {
            index = 3;
        }
        return index;
    }
    
    void Button::setClickEnabled(bool enabled)
    {
        Sprite::setClickEnabled(enabled);
        setButtonShowStatus(mIsEnabled ? ButtonState::Normal : ButtonState::Disable);
        
        // deal with button event
        if (mEventFuncMap.empty()) {
            mEventFuncMap[ClickEvent::eMoveIn] = [&](ClickEvent event, const Vector2& pos) {
                setButtonShowStatus(ButtonState::Highlight);
                if (mButtonFuncMap.find(ClickEvent::eMoveIn) != mButtonFuncMap.end()) {
                    mButtonFuncMap[ClickEvent::eMoveIn](event, pos);
                }
            };
            mEventFuncMap[ClickEvent::eUp] = [&](ClickEvent event, const Vector2& pos) {
                uint upState = (CursorManager::getSingleton().isCursorEnabled() ? ButtonState::Highlight : ButtonState::Normal);
                setButtonShowStatus(upState);
                if (mButtonFuncMap.find(ClickEvent::eUp) != mButtonFuncMap.end()) {
                    mButtonFuncMap[ClickEvent::eUp](event, pos);
                }
            };
            mEventFuncMap[ClickEvent::eDown] = [&](ClickEvent event, const Vector2& pos) {
                setButtonShowStatus(ButtonState::Down);
                if (mButtonFuncMap.find(ClickEvent::eDown) != mButtonFuncMap.end()) {
                    mButtonFuncMap[ClickEvent::eDown](event, pos);
                }
            };
            mEventFuncMap[ClickEvent::eDragIn] = [&](ClickEvent event, const Vector2& pos) {
                setButtonShowStatus(ButtonState::Down);
                if (mButtonFuncMap.find(ClickEvent::eDragIn) != mButtonFuncMap.end()) {
                    mButtonFuncMap[ClickEvent::eDragIn](event, pos);
                }
            };
            mEventFuncMap[ClickEvent::eMoveOut] = [&](ClickEvent event, const Vector2& pos) {
                setButtonShowStatus(ButtonState::Normal);
                if (mButtonFuncMap.find(ClickEvent::eMoveOut) != mButtonFuncMap.end()) {
                    mButtonFuncMap[ClickEvent::eMoveOut](event, pos);
                }
            };
            mEventFuncMap[ClickEvent::eDragOut] = [&](ClickEvent event, const Vector2& pos) {
                setButtonShowStatus(ButtonState::Normal);
                if (mButtonFuncMap.find(ClickEvent::eDragOut) != mButtonFuncMap.end()) {
                    mButtonFuncMap[ClickEvent::eDragOut](event, pos);
                }
            };
            mEventFuncMap[ClickEvent::eCancel] = [&](ClickEvent event, const Vector2& pos) {
                setButtonShowStatus(ButtonState::Normal);
                if (mButtonFuncMap.find(ClickEvent::eCancel) != mButtonFuncMap.end()) {
                    mButtonFuncMap[ClickEvent::eCancel](event, pos);
                }
            };
        }
    }
    
    void Button::setClickedAction(ControlListenerFunction func, ClickEvent type)
    {
        // deal with button show
        switch (type) {
            case ClickEvent::eMoveIn:
            case ClickEvent::eUp:
            case ClickEvent::eDown:
            case ClickEvent::eDragIn:
            case ClickEvent::eMoveOut:
            case ClickEvent::eDragOut:
            case ClickEvent::eCancel:
                mButtonFuncMap[type] = func;
                break;
            default:
                mEventFuncMap[type] = func;
                break;
        }
    }
}