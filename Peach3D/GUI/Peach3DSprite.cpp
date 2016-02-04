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
        if (texName && texName[0] == '#') {
            TextureFrame outFrame;
            if (ResourceManager::getSingleton().getTextureFrame(texName, outFrame)){
                return Sprite::create(outFrame);
            }
        }
        else {
            ITexture* texture = ResourceManager::getSingleton().addTexture(texName);
            return Sprite::create(TextureFrame(texture));
        }
        return nullptr;
    }
    
    Sprite* Sprite::create(const TextureFrame& frame)
    {
        Sprite* ns = nullptr;
        if (frame.tex) {
            ns = new Sprite();
            // add texture, content size also will be changed
            ns->setTexture(frame.tex);
            ns->setTextureRect(frame.rc);
            ns->resetCotentSizeWithTexture();
        }
        return ns;
    }
    
    Sprite::Sprite() : mIsUse9Scale(false), mIsGrayscale(false), mRenderTex(nullptr), mIsAutoResize(true),
        mIsEnabled(false), mIsClickZoomed(true), mCurrentZoomed(false)
    {
        // default to set alpha 1.0f for texture UI
        setAlpha(1.0f);
        // set default texture rect
        mTexRect = Rect(0.0f, 0.0f, 1.0f, 1.0f);
    }
    
    void Sprite::setTexture(ITexture* texture)
    {
        Peach3DAssert(texture, "Can't set null texture to sprite!");
        if (texture) {
            mRenderTex = texture;
            mIsRenderHashDirty = true;
            // reset content size, scale 9 or label not need set to texture size
            if (mIsAutoResize) {
                resetCotentSizeWithTexture();
            }
        }
    }
    
    void Sprite::setTextureRect(const Rect& texRect)
    {
        mTexRect = texRect;
        // reset content size, scale 9 or label not need set to texture size
        if (mIsAutoResize && mRenderTex) {
            resetCotentSizeWithTexture();
        }
    }
    
    void Sprite::resetCotentSizeWithTexture()
    {
        Vector2 texSize((float)mRenderTex->getWidth(), (float)mRenderTex->getHeight());
        Widget::setContentSize(texSize * mTexRect.size);
    }
    
    void Sprite::setClickEnabled(bool enabled)
    {
        // create or delete event listener
        if (!mIsEnabled && enabled) {
            EventDispatcher::getSingletonPtr()->addClickEventListener(this, [&](ClickEvent event, const std::vector<Vector2>& poss) {
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
                    mEventFuncMap[event](event, poss[0]);
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
    
    void Sprite::updateRenderingState(const std::string& extState)
    {
        if (mIsRenderHashDirty) {
            // set default program, then Widget::updateRenderingState will not change it
            if (!mRenderProgram) {
                mRenderProgram = ResourceManager::getSingleton().getPresetProgram(VertexType::Point2, "PosColorUVVerShader2D", "PosColorUVFragShader2D");
            }
            // sprite will not render if texture is null
            Widget::updateRenderingState(mRenderTex ? mRenderTex->getName() : "");
        }
    }
}