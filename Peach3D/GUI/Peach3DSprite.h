//
//  Peach3DSprite.h
//  Peach3DLib
//
//  Created by singoon.he on 12/23/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#ifndef PEACH3D_SPRITE_H
#define PEACH3D_SPRITE_H

#include "Peach3DCompile.h"
#include "Peach3DWidget.h"
#include "Peach3DTypes.h"

namespace Peach3D
{
    // redefine control event listener function, target not needed because c++11 Lambda
    typedef std::function<void(const Vector2&)> ControlListenerFunction;

    class PEACH3D_DLL Sprite : public Widget
    {
    public:
        /** Create sprite with no texture. */
        static Sprite* create();
        /** Create sprite, this will draw a texture, default size if texture rect size. */
        static Sprite* create(const char* texName, const Rect& coord=Rect(0.0f, 0.0f, 1.0f, 1.0f));
        /** Create sprite with texture and rect. */
        static Sprite* create(const TextureFrame& frame);
        
        /** Get is need rendering, not show if texture is nullptr. */
        virtual bool isNeedRender() { return Node::isNeedRender() && mRenderFrame.tex; }
        
        /** Show grayscale, this will use special program. */
        void setGrayscaleEnabled(bool enabled) { mIsGrayscale = enabled; }
        /** Get is grayscale enabled. */
        bool isGrayscaleEnabled() {return mIsGrayscale;}
        /** When content size called, show rect will not follow with texture. */
        void setContentSize(const Vector2& size) {mIsAutoResize = false; Widget::setContentSize(size);}
        /* Auto set content size to texture size. */
        void resetCotentSizeWithTexture();
        
        /** Default widget click event not enabled. */
        virtual void setClickEnabled(bool enabled);
        bool getClickEnabled() {return mIsEnabled;}
        /** Set is run scale action when touch down. */
        void setClickZoomed(bool zoomed) {mIsClickZoomed = zoomed;}
        bool getClickZoomed() {return mIsClickZoomed;}
        /** Set is use scale9 tyle. */
        void setScale9Enabled(bool enable);
        bool isScale9Enabled() { return mIsUse9Scale; }
        /** Set center sprite relative rect for scale9, default is Rect(1/3.f, 1/3.f, 1/3.f, 1/3.f). */
        void setCenterRect(const Rect& rc);
        const Rect& getCenterRect() { return mCenterRect; }
        /** Set clicked event call func. */
        virtual void setClickedAction(ControlListenerFunction func, ClickEvent type = ClickEvent::eClicked);
        
        /** Reset texture and uv coord. */
        void setTextureFrame(const TextureFrame& frame);
        void setTextureRect(const Rect& rc) { setTextureFrame(TextureFrame(mRenderFrame.tex, rc)); }
        const TextureFrame& getTextureFrame()const { return mRenderFrame; }
        
    protected:
        Sprite();
        virtual ~Sprite() {}
        
        /** Update rendering attributes, about world rect/rotate/scale... */
        virtual void updateRenderingAttributes(float lastFrameTime);
        /** Update rendering state, preset program and calculate hash code. */
        virtual void updateRenderingState(const std::string& extState="");
        
    protected:
        bool            mIsGrayscale;   // is need show grayscale
        bool            mIsAutoResize;  // is auto resize to texture size
        TextureFrame    mRenderFrame;   // render frame, include texture and coord
        bool            mIsUse9Scale;   // is need scale9 type
        Rect            mCenterRect;    // center sprite rect for scale9
        Sprite          *m9Child[9];    // child sprites when using scale9
        
        bool            mIsEnabled;     // is event enabled
        bool            mIsClickZoomed; // is auto zoom when clicked down
        bool            mCurrentZoomed; // current widget is zoomed
        std::map<ClickEvent, ControlListenerFunction> mEventFuncMap;
    };
}

#endif /* defined(PEACH3D_SPRITE_H) */
