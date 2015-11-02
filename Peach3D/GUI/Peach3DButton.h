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
#define ButtonStatusNormal  0x01
#define ButtonStatusHighlight  0x02 // high light satus, just for mouse event
#define ButtonStatusDown    0x04    // touch down or mouse click down
#define ButtonStatusDisable 0x08    // default is normal's grep effect
#define ButtonStatusCount   4       // button status count
    
    class PEACH3D_DLL Button : public Sprite
    {
    public:
        /** Create button, static method. */
        static Button* create(const std::string& normal, const std::string& down="", const std::string& highlight="",
                              const std::string& disable="");
        static Button* create();
        
        /** Set multi status texture use combination sign "|". */
        void setTextureForStatus(ITexture* tex, uint status);
        /** Return status's texture. */
        ITexture* getTextureForStatus(uint status) { return mStatusTexs[getStatusIndex(status)]; }
        /** Set multi status rect use combination sign "|". */
        void setTextureRectForStatus(Rect rect, uint status);
        /** Return status's texture rect. */
        Rect getTextureRectForStatus(uint status) { return mStatusTexRects[getStatusIndex(status)]; }
        
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
        
        uint        mCurStatus;
        ITexture*   mStatusTexs[ButtonStatusCount];
        Rect        mStatusTexRects[ButtonStatusCount];
        std::map<ClickEvent, ControlListenerFunction> mButtonFuncMap;
    };
}

#endif // PEACH3D_BUTTON_H
