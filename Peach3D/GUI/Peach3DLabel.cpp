//
//  Peach3DLabel.cpp
//  Peach3DLib
//
//  Created by singoon.he on 1/12/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//
#include "Peach3DLabel.h"
#include "Peach3DUtils.h"
#include "Peach3DIPlatform.h"
#include "Peach3DResourceManager.h"

namespace Peach3D
{
    Label* Label::create(const std::string& text, float fontSize, const std::string& font, TextHAlignment hAlign, TextVAlignment vAlign)
    {
        Label* newLabel = new Label();
        newLabel->setText(text);
        newLabel->setFontName(font);
        newLabel->setFontSize(fontSize);
        newLabel->setHAlignment(hAlign);
        newLabel->setVAlignment(vAlign);
        return newLabel;
    }
    
    Label* Label::create()
    {
        Label* newLabel = new Label();
        return newLabel;
    }
    
    Label::~Label()
    {
        // delete texture, it is almost impossible to reuse
        if (mRenderFrame.tex) {
            ResourceManager::getSingleton().deleteTexture(mRenderFrame.tex);
        }
    }
    
    void Label::setContentSize(const Vector2& size)
    {
        if (size != mRect.size) {
            Sprite::setContentSize(size);
            mIsTexDirty = true;
            // text texture will use this size allways
            mIsDimUserAssigned = true;
        }
    }
    
    void Label::setScale(const Vector2& scale)
    {
        if (scale != mScale) {
            Sprite::setScale(scale);
            mIsTexDirty = true;
        }
    }
    
    void Label::setText(const std::string& text)
    {
        if (text != mText) {
            mText = text;
            mIsTexDirty = true;
        }
    }
    
    void Label::setFontSize(float fontSize)
    {
        if (!FLOAT_EQUAL(fontSize, mFontSize)) {
            mFontSize = fontSize;
            mIsTexDirty = true;
        }
    }
    
    void Label::setFontName(const std::string& font)
    {
        if (font != mFontName) {
            mFontName = font;
            mIsTexDirty = true;
        }
    }
    
    void Label::setFillColor(const Color3& color)
    {
        if (!FLOAT_EQUAL(mFillColor.r, color.r) || !FLOAT_EQUAL(mFillColor.g, color.g) || !FLOAT_EQUAL(mFillColor.b, color.b)) {
            mFillColor = color;
            mIsTexDirty = true;
        }
    }
    
    void Label::setVAlignment(TextVAlignment align)
    {
        if (align != mVAlign) {
            mVAlign = align;
            mIsTexDirty = true;
        }
    }
    
    void Label::setHAlignment(TextHAlignment align)
    {
        if (align != mHAlign) {
            mHAlign = align;
            mIsTexDirty = true;
        }
    }
    
    void Label::setImageVAlignment(TextVAlignment align)
    {
        if (align != mImageVAlign) {
            mImageVAlign = align;
            mIsTexDirty = true;
        }
    }
    
    void Label::formatText()
    {
        if (mIsTexDirty && mText.size() > 0) {
            mClickRectMap.clear();
            std::vector<char> curText;
            Color4 curColor = mFillColor;
            bool curClickEnabled = false;
            
            // r text with "{}"
            std::vector<LabelStageTextInfo> stageList;
            size_t len = mText.size();
            int signStartIndex = -1;
            for (size_t i=0; i<len;)
            {
                char textc = mText[i];
                if (textc=='{') {
                    // use two '{' instead of one real '{'
                    if ((i+1)<len && mText[i+1]=='{') {
                        i++;
                    }
                    else {
                        // add current text, valid format length must > 2 "{}"
                        if (curText.size() > 0 && (i+2)<len) {
                            addTextToStageList(std::string(curText.begin(), curText.end()), curColor, curClickEnabled, &stageList);
                            // set default info
                            curClickEnabled = false;
                            curText.clear();
                        }
                        signStartIndex = (int)i;
                    }
                    curText.push_back(textc);
                }
                else if (textc=='}' && signStartIndex>=0 && i>size_t(signStartIndex+1)) {
                    std::string stageText = mText.substr(signStartIndex+1, i-signStartIndex-1);
                    // pause stage text
                    
                    std::vector<std::string> lineList = Utils::split(stageText, ',');
                    Color4 outColor;
                    for (auto line : lineList) {
                        std::string trimedStr = Utils::trim(line);
                        if (trimedStr.compare("click") == 0) {
                            curClickEnabled = true;
                        }
                        else if (convertStringToColor4(trimedStr, &outColor)) {
                            curColor = outColor;
                        }
                        // default is image
                        else {
                            LabelStageTextInfo imageInfo;
                            imageInfo.image = trimedStr;
                            stageList.push_back(imageInfo);
                        }
                    }
                    signStartIndex = -1;
                    curText.clear();
                }
                else {
                    curText.push_back(textc);
                }
                i++;
            }
            // add last text
            if (curText.size() > 0) {
                addTextToStageList(std::string(curText.begin(), curText.end()), curColor, curClickEnabled, &stageList);
            }
            
            // calc real string
            mRealText.clear();
            for (auto info : stageList) {
                mRealText = mRealText + info.text;
            }
            
            // replace string texture
            float minScale = std::min(mWorldScale.x, mWorldScale.y);
            LabelTextDefined defined;
            defined.font = mFontName;
            defined.fontSize = mFontSize * minScale;
            defined.hAlign = mHAlign;
            defined.vAlign = mVAlign;
            defined.imageVAlign = mImageVAlign;
            // if content size not set, system will set size automatically
            if (mIsDimUserAssigned) {
                defined.dim = mWorldAnchorRect.size;
            }
            
            size_t oldClickedCount = mClickRectMap.size();
            mClickRectMap.clear();
            ITexture* labelTex = IPlatform::getSingleton().getTextTexture(stageList, defined, mClickRectMap);
            if (labelTex) {
                if (mRenderFrame.tex) {
                    ResourceManager::getSingleton().deleteTexture(mRenderFrame.tex);
                }
                setTextureFrame(TextureFrame(labelTex));
                // also reset content size if system calc size
                if (!mIsDimUserAssigned) {
                    Sprite::setContentSize(Vector2((float)labelTex->getWidth() / minScale, (float)labelTex->getHeight() / minScale));
                }
            }
            
            // add clicked func or delete it
            if (oldClickedCount == 0 && mClickRectMap.size() > 0) {
                addLabelClickedEventFunc();
            }
            else if (oldClickedCount > 0 && mClickRectMap.size() == 0) {
                EventDispatcher::getSingletonPtr()->deleteClickEventListener(this);
            }
            
            // need update render state and hash code (texture changed)
            mIsRenderDirty = true;
            // render hash code also need recalc because texture changed
            mIsRenderHashDirty = true;
            // set format over
            mIsTexDirty = false;
        }
    }
    
    void Label::updateRenderingState(const std::string& extState)
    {
        // create label texure, mWorldScale is need here
        formatText();
        // current texture is ready, then update render state
        Sprite::updateRenderingState();
    }
    
    bool Label::convertStringToColor4(const std::string& colorString, Color4* outColor)
    {
        bool success = false;
        // can't find image sign '.'
        if (colorString.size() >= 6 && colorString.find(',')==std::string::npos) {
            bool errorFormat = false;
            unsigned int numColor=0;
            float *single[3] = {&outColor->r, &outColor->g, &outColor->b};
            for (auto i=0; i<3; ++i) {
                std::string sliceString = colorString.substr(i*2, 2);
                for (auto j=0; j<2; ++j) {
                    if ((sliceString[j]<48) ||
                        (sliceString[j]>57 && sliceString[j]<65) ||
                        (sliceString[j]>70 && sliceString[j]<97) ||
                        (sliceString[j]>102)) {
                        errorFormat = true;
                    }
                    else if (sliceString[j]>=97 && sliceString[j]<=102) {
                        // convert to upper
                        sliceString[j] = sliceString[j] - 32;
                    }
                }
                if (errorFormat) {
                    break;
                }
                else {
                    sscanf(sliceString.c_str(),"%X", &numColor);
                    *single[i] = numColor/255.0f;
                }
            }
            if (errorFormat == false) {
                success = true;
            }
        }
        return success;
    }
    
    void Label::addTextToStageList(const std::string& text, const Color4& color, bool click, std::vector<LabelStageTextInfo>* infoList)
    {
        std::vector<char> curText;
        for (auto textc : text) {
            if (textc == '\n') {
                std::vector<std::string> infoTextList;
                if (curText.size() > 0) {
                    infoTextList.push_back(std::string(curText.begin(), curText.end()));
                }
                infoTextList.push_back("\n");
                // add all text to list, include '\n'
                for (auto textStr : infoTextList) {
                    if (textStr.size() > 0) {
                        LabelStageTextInfo info;
                        info.text = textStr;
                        info.color = color;
                        info.clickEnabled = click;
                        infoList->push_back(info);
                    }
                }
                curText.clear();
            }
            else {
                curText.push_back(textc);
            }
        }
        if (curText.size() > 0) {
            LabelStageTextInfo info;
            info.text = std::string(curText.begin(), curText.end());
            info.color = color;
            info.clickEnabled = click;
            infoList->push_back(info);
        }
    }
    
    void Label::addLabelClickedEventFunc()
    {
        // register widget click event, trigger label clicked event
        EventDispatcher::getSingletonPtr()->addClickEventListener(this, [&](ClickEvent event, const std::vector<Vector2>& pos){
            if (mClickedFunc) {
                if (event == ClickEvent::eDown) {
                    // is pos in label clicked rect
                    // save the key for the rect
                    mClickDownKey = "";
                }
                else if (event == ClickEvent::eUp) {
                    // calc current pos
                    std::string curKey = "";
                    if (mClickDownKey == curKey) {
                        mClickedFunc(mClickDownKey);
                    }
                    mClickDownKey.clear();
                }
            }
        });
    }
}