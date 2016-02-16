//
//  Peach3DButton.h
//  Peach3DLib
//
//  Created by singoon.he on 9/4/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#ifndef PEACH3D_BUTTON_H
#define PEACH3D_BUTTON_H

#include "Peach3DCompile.h"
#include "Peach3DSprite.h"
#include "Peach3DLabel.h"

namespace Peach3D
{
    // define button state, use combined
    namespace ButtonState
    {
        const uint Normal = 0x0001;
        const uint Highlight = 0x0002;  // high light state, just for mouse event
        const uint Down = 0x0004;       // touch down or mouse click down
        const uint Disable  = 0x0008;   // default is normal's grep effect
        const uint Count  = 4;          // button status count
    }
    
    class PEACH3D_DLL Button : public Sprite
    {
    public:
        /** Create button, static method. */
        static Button* create(const char* normal, const char* down="", const char* highlight="",
                              const char* disable="");
        static Button* create();
        
        /** Set multi status texture frame use combination sign "|". */
        void setTextureFrameForStatus(const TextureFrame& frame, uint status);
        /** Set multi status texture rect use combination sign "|". */
        void setTextureRectForStatus(const Rect& rc, uint status);
        /** Return status's texture frame. */
        const TextureFrame& getTextureFrameForStatus(uint status) { return mStatusFrames[getStatusIndex(status)]; }
        
        /** Auto reset title label position. */
        virtual void setContentSize(const Vector2& size);
        /** Return title label if exist. */
        Label* getTitleLabel() {return mTitleLabel;}
        void setTitleText(const std::string& title);
        const char* getTitleText()const {return mTitleLabel ? mTitleLabel->getText().c_str() : nullptr;}
        void setTitleFontName(const std::string& name);
        const char* getTitleFontName()const {return mTitleLabel ? mTitleLabel->getFontName().c_str() : nullptr;}
        void setTitleFontSize(float size);
        float getTitleFontSize(){return mTitleLabel ? mTitleLabel->getFontSize() : 0.0f;}
        void setTitleColor(const Color4& color);
        Color4 getTitleColor() {return mTitleLabel ? mTitleLabel->getFillColor() : Color4();}
        /** Set title position relative to button size, (0.5, 0.5) is center of button. */
        void setTitleOffset(const Vector2& offset);
        /** Get title position relative to button size. */
        const Vector2& getTitleAnchorOffset() {return mTitleOffset;}
        
        /** This will affect clicked and change to gray if disable image not be set. */
        virtual void setClickEnabled(bool enabled);
        /** Set click action, also change status images. */
        virtual void setClickedAction(ControlListenerFunction func, ClickEvent type = ClickEvent::eClicked);
        
    public:
        Button();
        virtual ~Button() {}
        
    protected:
        // set current button show status
        void setButtonShowStatus(uint status);
        // get texture and rect index for show status
        uint getStatusIndex(uint status);
        
    private:
        Label*      mTitleLabel;
        Vector2     mTitleOffset;
        
        uint            mCurStatus;
        TextureFrame    mStatusFrames[ButtonState::Count];
        std::map<ClickEvent, ControlListenerFunction> mButtonFuncMap;
    };
}

#endif // PEACH3D_BUTTON_H
