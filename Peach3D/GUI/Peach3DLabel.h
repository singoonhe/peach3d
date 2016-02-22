//
//  Peach3DLabel.h
//  Peach3DLib
//
//  Created by singoon.he on 1/12/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#ifndef PEACH3D_LABEL_H
#define PEACH3D_LABEL_H

#include "Peach3DCompile.h"
#include "Peach3DSprite.h"

namespace Peach3D
{
    // redefine label clicked event listener function, it's too long
    typedef std::function<void(const std::string&)> LabelClickedListenerFunction;
    
    class PEACH3D_DLL Label : public Sprite
    {
    public:
        /** Create label, static method. */
        static Label* create(const std::string& text, float fontSize = 20.0f, const std::string& font="",
                             TextHAlignment hAlign=TextHAlignment::eLeft, TextVAlignment vAlign=TextVAlignment::eBottom);
        static Label* create();
        
        /** Set label content size, will rebuild label texture. */
        virtual void setContentSize(const Vector2& size);
        /** Set scaling, also set texture need update. */
        virtual void setScale(const Vector2& scale);
        
        /** Get real label text, ignore all modifier. */
        const std::string& getRealText()const {return mRealText;}
        void setText(const std::string& text);
        const std::string& getText()const {return mText;}
        void setFontSize(float fontSize);
        float getFontSize() {return mFontSize;}
        void setFontName(const std::string& font);
        const std::string& getFontName()const {return mFontName;}
        void setVAlignment(TextVAlignment align);
        TextVAlignment getVAlignment() {return mVAlign;}
        void setHAlignment(TextHAlignment align);
        TextHAlignment getHAlignment() {return mHAlign;}
        
        /** Set text fill color. This will effect start text color, Widget::setColor is also valid. */
        void setFillColor(const Color3& color);
        /** Get text fill color. */
        const Color3& getFillColor()const {return mFillColor;}
        /** Set label image vertical alignment, deafult is Center. */
        void setImageVAlignment(TextVAlignment align);
        /** Get label image vertical alignment. */
        TextVAlignment getImageVAlignment() {return mImageVAlign;}
        /** Set clicked func. */
        void setClickedFunc(LabelClickedListenerFunction func) { mClickedFunc = func; }
        //! format text
        void formatText();
        
    protected:
        Label() : mIsTexDirty(true), mClickedFunc(nullptr), mVAlign(TextVAlignment::eBottom),mHAlign(TextHAlignment::eLeft),mFontSize(0.0f), mFillColor(1.0f, 1.0f, 1.0f), mIsDimUserAssigned(false), mImageVAlign(TextVAlignment::eCenter) { mIsAutoResize = false; }
        virtual ~Label();
        
        /** Format text and update render state. */
        virtual void updateRenderingState(const std::string& extState="");
        
        // deal label clicked event
        void addLabelClickedEventFunc();
        // convert color string to Color4
        bool convertStringToColor4(const std::string& colorString, Color4* outColor);
        // add one LabelStageTextInfo to stage list
        void addTextToStageList(const std::string& text, const Color4& color, bool click, std::vector<LabelStageTextInfo>* infoList);
        
    protected:
        TextVAlignment  mVAlign;
        TextHAlignment  mHAlign;
        std::string     mText;
        std::string     mRealText;
        std::string     mFontName;
        float           mFontSize;
        Color3          mFillColor;
        TextVAlignment  mImageVAlign;
        bool            mIsDimUserAssigned; // is label's dim assigned by user
        bool            mIsTexDirty;        // is label texture need update
        
        std::string                                 mClickDownKey;  // current click down text key
        std::map<std::string, std::vector<Rect>>    mClickRectMap;  // text click valid rect list
        LabelClickedListenerFunction                mClickedFunc;   // clicked listener function
    };
}

#endif /* defined(PEACH3D_LABEL_H) */
