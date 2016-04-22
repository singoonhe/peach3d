//
//  Peach3DLayoutManager.cpp
//  Peach3DLib
//
//  Created by singoon.he on 5/17/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#include "Peach3DLayoutManager.h"
#include "Peach3DSceneManager.h"
#include "Peach3DResourceManager.h"
#include "Peach3DEventDispatcher.h"
#include "Peach3DUtils.h"
#include "Peach3DLogPrinter.h"
#include "Peach3DButton.h"
#include "Peach3DLabel.h"
#include "Peach3DSprite.h"

namespace Peach3D
{
    IMPLEMENT_SINGLETON_STATIC(LayoutManager);
    
    LayoutManager::LayoutManager() : mIsLandscape(false), mLocalizedFunc(nullptr)
    {
        const char* attrStrList[] = {"Widget", "Sprite", "Label", "Button", "Layout", "NormalTexture", "DownTexture", "HighlightTexture",
            "DisableTexture", "NormalCoord", "DownCoord", "HighlightCoord", "DisableCoord", "Anchor", "BindCorner", "AutoPosition",
            "Position", "AutoSize", "Scale", "Text", "FontSize", "HAlignment", "VAlignment", "TitleOffset",
            "SwallowEvents", "FillColor", "Alpha", "Color", "Texture", "Coord", "Visible" };
        for (auto i = 0; i < static_cast<int>(LayoutAttrType::eCount); ++i) {
            mLoadAttrMap.insert(std::pair<std::string, LayoutAttrType>(attrStrList[i], static_cast<LayoutAttrType>(i)));
        }
    }

    bool LayoutManager::loadLayout(const char* file, Widget* parent, const LayoutVarBindFunction& bindFunc, const LayoutLoadOverFunction& overFunc)
    {
        std::string fileName = file;
        std::string ext = fileName.substr(fileName.rfind('.') == std::string::npos ? fileName.length() : fileName.rfind('.') + 1);
        Peach3DAssert(ext.compare("plt") == 0, "Not supported layout format file");
        
        bool loadRes = false;
        // layout filename extension must be 'plt'
        if (ext.compare("plt") == 0) {
            ulong fileLength = 0;
            uchar* fileData = ResourceManager::getSingleton().getFileData(file, &fileLength);
            if (fileData && fileLength > 0) {
                loadRes = loadLayout(fileData, fileLength, parent, bindFunc, overFunc);
                free(fileData);
            }
            
            if (loadRes) {
                Peach3DLog(LogLevel::eInfo, "Load layout file \"%s\" success", file);
            }
        }
        return loadRes;
    }
    
    bool LayoutManager::loadLayout(const uchar* content, ulong len, Widget* parent, const LayoutVarBindFunction& bindFunc, const LayoutLoadOverFunction& overFunc)
    {
        bool loadRes = false;
        XMLDocument readDoc;
        if (readDoc.Parse((const char*)content, len) == XML_SUCCESS) {
            do {
                XMLElement* layoutEle = readDoc.FirstChildElement("Layout");
                IF_BREAK(!layoutEle, "Layout file format error");

                // read scene node element
                XMLElement* nodeEle = layoutEle->FirstChildElement();
                Widget* rootNode = parent ? parent : SceneManager::getSingleton().getRootWidget();
                while (nodeEle) {
                    // create new widget
                    Widget* childNode = createTypeNameWidget(nodeEle->Name(), nodeEle->Attribute("name"), rootNode, bindFunc);
                    if (childNode) {
                        loadWidgetData(childNode, nodeEle, rootNode, bindFunc);
                    }
                    
                    nodeEle = nodeEle->NextSiblingElement();
                }
                
                // call load over function
                const char* layoutName = layoutEle->Attribute("name");
                if (overFunc) {
                    overFunc(layoutName ? layoutName : "");
                }
                
                loadRes = true;
            } while (0);
        }
        return loadRes;
    }
    
    Widget* LayoutManager::createTypeNameWidget(const char* typeName, const char* nodeName, Widget* parentNode, const LayoutVarBindFunction& bindFunc)
    {
        Widget* newNode = nullptr;
        // create type widget
        switch (mLoadAttrMap[typeName])
        {
            case LayoutAttrType::eButton:
                newNode = Button::create();
                break;
            case LayoutAttrType::eWidget:
                newNode = Widget::create();
                break;
            case LayoutAttrType::eSprite:
                newNode = Sprite::create();
                break;
            case LayoutAttrType::eLabel:
                newNode = Label::create();
                break;
            case LayoutAttrType::eLayout:
                if (nodeName) {
                    loadLayout(nodeName, parentNode, bindFunc);
                }
                break;
            default:
                break;
        }
        return newNode;
    }
    
    void LayoutManager::loadWidgetData(Widget* newNode, const XMLElement* nodeEle, Widget* parentNode, const LayoutVarBindFunction& bindFunc)
    {
        const char* nodeName = nodeEle->Attribute("name");
        // cache frame file if needed
        const char* frameName = nodeEle->Attribute("frame");
        if (frameName) {
            ResourceManager::getSingleton().addTextureFrames(frameName, nullptr);
        }
        // define const type and names
        const static std::string bindPosNameKeys[] = {"LeftBottom", "LeftCenter", "LeftTop", "CenterBottom", "Center", "CenterTop", "RightBottom", "RightCenter", "RightTop"};
        const static Vector2 bindPosNameValues[] = {Vector2LeftBottom, Vector2LeftCenter, Vector2LeftTop, Vector2CenterBottom, Vector2Center, Vector2CenterTop, Vector2RightBottom, Vector2RightCenter, Vector2RightTop};
        const static std::string bindScaleNames[] = {"Fix", "Min", "Width", "Height", "Max"};
        const static AutoScaleType bindScaleValues[] = {AutoScaleType::eFix, AutoScaleType::eMin, AutoScaleType::eWidth, AutoScaleType::eHeight, AutoScaleType::eMax};
        static std::map<std::string, TextHAlignment> bindHAlignMap = {std::pair<std::string, TextHAlignment>("Left", TextHAlignment::eLeft), std::pair<std::string, TextHAlignment>("Center", TextHAlignment::eCenter), std::pair<std::string, TextHAlignment>("Right", TextHAlignment::eRight)};
        static std::map<std::string, TextVAlignment> bindVAlignMap = {std::pair<std::string, TextVAlignment>("Top", TextVAlignment::eTop), std::pair<std::string, TextVAlignment>("Center", TextVAlignment::eCenter), std::pair<std::string, TextVAlignment>("Bottom", TextVAlignment::eBottom)};

        // read attrs element
        const XMLElement* attrEle = nodeEle->FirstChildElement();
        while (attrEle) {
            const char* attrText = attrEle->GetText();
            LayoutAttrType attrType = mLoadAttrMap[attrEle->Name()];
            if (!attrText) {
                // deal with control attribute
                if (attrType <= LayoutAttrType::eLayout) {
                    Widget* childNode = createTypeNameWidget(attrEle->Name(), attrEle->Attribute("name"), newNode, bindFunc);
                    if (childNode) {
                        loadWidgetData(childNode, attrEle, newNode, bindFunc);
                    }
                }
                attrEle = attrEle->NextSiblingElement();
                continue;
            }
            
            switch (attrType) {
                case LayoutAttrType::eNormalTexture:
                    if (dynamic_cast<Button*>(newNode)) {
                        TextureFrame outFrame;
                        ResourceManager::getSingleton().getTextureFrame(attrText, &outFrame);
                        dynamic_cast<Button*>(newNode)->setTextureFrameForStatus(outFrame, ButtonState::Normal);
                    }
                    break;
                case LayoutAttrType::eDownTexture:
                    if (dynamic_cast<Button*>(newNode)) {
                        TextureFrame outFrame;
                        ResourceManager::getSingleton().getTextureFrame(attrText, &outFrame);
                        dynamic_cast<Button*>(newNode)->setTextureFrameForStatus(outFrame, ButtonState::Down);
                    }
                    break;
                case LayoutAttrType::eHighlightTexture:
                    if (dynamic_cast<Button*>(newNode)) {
                        TextureFrame outFrame;
                        ResourceManager::getSingleton().getTextureFrame(attrText, &outFrame);
                        dynamic_cast<Button*>(newNode)->setTextureFrameForStatus(outFrame, ButtonState::Highlight);
                    }
                    break;
                case LayoutAttrType::eDisableTexture:
                    if (dynamic_cast<Button*>(newNode)) {
                        TextureFrame outFrame;
                        ResourceManager::getSingleton().getTextureFrame(attrText, &outFrame);
                        dynamic_cast<Button*>(newNode)->setTextureFrameForStatus(outFrame, ButtonState::Disable);
                    }
                    break;
                case LayoutAttrType::eNormalCoord:
                    if (dynamic_cast<Button*>(newNode)) {
                        Rect attrRect;
                        sscanf(attrText, "%f,%f,%f,%f", &attrRect.pos.x, &attrRect.pos.y, &attrRect.size.x, &attrRect.size.y);
                        dynamic_cast<Button*>(newNode)->setTextureRectForStatus(attrRect, ButtonState::Normal);
                    }
                    break;
                case LayoutAttrType::eDownCoord:
                    if (dynamic_cast<Button*>(newNode)) {
                        Rect attrRect;
                        sscanf(attrText, "%f,%f,%f,%f", &attrRect.pos.x, &attrRect.pos.y, &attrRect.size.x, &attrRect.size.y);
                        dynamic_cast<Button*>(newNode)->setTextureRectForStatus(attrRect, ButtonState::Down);
                    }
                    break;
                case LayoutAttrType::eHighlightCoord:
                    if (dynamic_cast<Button*>(newNode)) {
                        Rect attrRect;
                        sscanf(attrText, "%f,%f,%f,%f", &attrRect.pos.x, &attrRect.pos.y, &attrRect.size.x, &attrRect.size.y);
                        dynamic_cast<Button*>(newNode)->setTextureRectForStatus(attrRect, ButtonState::Highlight);
                    }
                    break;
                case LayoutAttrType::eDisableCoord:
                    if (dynamic_cast<Button*>(newNode)) {
                        Rect attrRect;
                        sscanf(attrText, "%f,%f,%f,%f", &attrRect.pos.x, &attrRect.pos.y, &attrRect.size.x, &attrRect.size.y);
                        dynamic_cast<Button*>(newNode)->setTextureRectForStatus(attrRect, ButtonState::Disable);
                    }
                    break;
                case LayoutAttrType::eAnchor: {
                    Vector2 attrAnchor;
                    sscanf(attrText, "%f,%f", &attrAnchor.x, &attrAnchor.y);
                    newNode->setAnchorPoint(attrAnchor);
                }
                    break;
                case LayoutAttrType::eBindCorner:
                    // <BindCorner>0, 0.5</BindCorner>
                    if (strchr(attrText, ',')) {
                        Vector2 corner;
                        sscanf(attrText, "%f,%f", &corner.x, &corner.y);
                        newNode->setBindCorner(corner);
                    }
                    else {
                        // <BindCorner>Center</BindCorner>
                        for (size_t i = 0; i < LAYOUT_INNER_POSITION_COUNT; ++i) {
                            if (bindPosNameKeys[i].compare(attrText) == 0) {
                                newNode->setBindCorner(bindPosNameValues[i]);
                                break;
                            }
                        }
                    }
                    break;
                case LayoutAttrType::eAutoPosition: {
                    std::vector<std::string> attrList = Utils::split(attrText, ',');
                    if (attrList.size() >= 2) {
                        AutoScaleType scaleX = AutoScaleType::eFix, scaleY = AutoScaleType::eFix;
                        for (size_t i = 0; i < 2; ++i) {
                            std::string termAttr = Utils::trim(attrList[i]);
                            for (size_t j = 0; j < LAYOUT_SCALE_TYPE_COUNT; ++j) {
                                if (bindScaleNames[j] == termAttr) {
                                    if (i == 0) {
                                        scaleX = bindScaleValues[j];
                                    }
                                    else if (i == 1) {
                                        scaleY = bindScaleValues[j];
                                    }
                                    break;
                                }
                            }
                        }
                        newNode->setPosScaleType(scaleX, scaleY);
                    }
                }
                    break;
                case LayoutAttrType::ePosition: {
                    Vector2 pos;
                    sscanf(attrText, "%f,%f", &pos.x, &pos.y);
                    newNode->setPosition(pos);
                }
                    break;
                case LayoutAttrType::eAutoSize: {
                    std::vector<std::string> attrList = Utils::split(attrText, ',');
                    if (attrList.size() >= 2) {
                        AutoScaleType scaleX = AutoScaleType::eFix, scaleY = AutoScaleType::eFix;
                        for (size_t i = 0; i < 2; ++i) {
                            std::string termAttr = Utils::trim(attrList[i]);
                            for (size_t j = 0; j < LAYOUT_SCALE_TYPE_COUNT; ++j) {
                                if (bindScaleNames[j] == termAttr) {
                                    if (i == 0) {
                                        scaleX = bindScaleValues[j];
                                    }
                                    else if (i == 1) {
                                        scaleY = bindScaleValues[j];
                                    }
                                    break;
                                }
                            }
                        }
                        newNode->setSizeScaleType(scaleX, scaleY);
                    }
                }
                    break;
                case LayoutAttrType::eScale: {
                    Vector2 scale;
                    sscanf(attrText, "%f,%f", &scale.x, &scale.y);
                    newNode->setScale(scale);
                }
                    break;
                case LayoutAttrType::eText: {
                    std::string localizedStr = attrText;
                    if (mLocalizedFunc) {
                        localizedStr = mLocalizedFunc(localizedStr);
                    }
                    if (dynamic_cast<Label*>(newNode)) {
                        dynamic_cast<Label*>(newNode)->setText(localizedStr);
                    }
                    else if (dynamic_cast<Button*>(newNode)) {
                        dynamic_cast<Button*>(newNode)->setTitleText(localizedStr);
                    }
                }
                    break;
                case LayoutAttrType::eFontSize: {
                    float fontSize = 0.f;
                    sscanf(attrText, "%f", &fontSize);
                    if (dynamic_cast<Label*>(newNode)) {
                        dynamic_cast<Label*>(newNode)->setFontSize(fontSize);
                    }
                    else if (dynamic_cast<Button*>(newNode)) {
                        dynamic_cast<Button*>(newNode)->setTitleFontSize(fontSize);
                    }
                }
                    break;
                case LayoutAttrType::eHAlignment:
                    if (dynamic_cast<Label*>(newNode) && bindHAlignMap.find(attrText) != bindHAlignMap.end()) {
                        dynamic_cast<Label*>(newNode)->setHAlignment(bindHAlignMap[attrText]);
                    }
                    break;
                case LayoutAttrType::eVAlignment:
                    if (dynamic_cast<Label*>(newNode) && bindVAlignMap.find(attrText) != bindVAlignMap.end()) {
                        dynamic_cast<Label*>(newNode)->setVAlignment(bindVAlignMap[attrText]);
                    }
                    break;
                case LayoutAttrType::eTitleOffset:
                    if (dynamic_cast<Button*>(newNode)) {
                        Vector2 attrOffset;
                        sscanf(attrText, "%f,%f", &attrOffset.x, &attrOffset.y);
                        dynamic_cast<Button*>(newNode)->setTitleOffset(attrOffset);
                    }
                    break;
                case LayoutAttrType::eSwallowEvents:
                    if (strcmp(attrText, "true") == 0) {
                        // register null listener, just swallow events.
                        EventDispatcher::getSingletonPtr()->addClickEventListener(newNode,
                                                                                  [&](ClickEvent event, const std::vector<Vector2>& poss) {});
                    }
                    break;
                case LayoutAttrType::eFillColor: {
                    Color3 fillColor;
                    sscanf(attrText, "%f,%f,%f", &fillColor.r, &fillColor.g, &fillColor.b);
                    if (dynamic_cast<Label*>(newNode)) {
                        dynamic_cast<Label*>(newNode)->setFillColor(fillColor);
                    }
                    else if (dynamic_cast<Button*>(newNode)) {
                        dynamic_cast<Button*>(newNode)->setTitleColor(fillColor);
                    }
                }
                    break;
                case LayoutAttrType::eAlpha:{
                    float attrAlpha;
                    sscanf(attrText, "%f", &attrAlpha);
                    newNode->setAlpha(attrAlpha);
                }
                    break;
                case LayoutAttrType::eColor: {
                    Color3 attrColor;
                    sscanf(attrText, "%f,%f,%f", &attrColor.r, &attrColor.g, &attrColor.b);
                    newNode->setColor(attrColor);
                }
                    break;
                case LayoutAttrType::eTexture:
                    if (dynamic_cast<Sprite*>(newNode)) {
                        TextureFrame outFrame;
                        ResourceManager::getSingleton().getTextureFrame(attrText, &outFrame);
                        dynamic_cast<Sprite*>(newNode)->setTextureFrame(outFrame.tex);
                    }
                    break;
                case LayoutAttrType::eCoord:
                    if (dynamic_cast<Sprite*>(newNode)) {
                        Rect attrRect;
                        sscanf(attrText, "%f,%f,%f,%f", &attrRect.pos.x, &attrRect.pos.y, &attrRect.size.x, &attrRect.size.y);
                        dynamic_cast<Sprite*>(newNode)->setTextureRect(attrRect);
                    }
                    break;
                case LayoutAttrType::eVisible:
                    newNode->setVisible(strcmp(attrText, "true") == 0);
                    break;
                default:
                    break;
            }
            
            attrEle = attrEle->NextSiblingElement();
        }
        // add new widget to parent
        parentNode->addChild(newNode);
        
        // set tag name and call bind function
        if (nodeName) {
            newNode->setName(nodeName);
            if (bindFunc) {
                bindFunc(nodeName, newNode);
            }
        }
    }

    void LayoutManager::setDesignSize(float width, float height)
    {
        mDesignScreenSize = Vector2(width, height);
        
        // calc min scale, width scale, height scale
        float minSize = std::min(mDesignScreenSize.x, mDesignScreenSize.y), maxSize = std::max(mDesignScreenSize.x, mDesignScreenSize.y);
        mWidthScale = mScreenSize.x / (mIsLandscape ? maxSize : minSize);
        mHeightScale = mScreenSize.y / (mIsLandscape ? minSize : maxSize);
        mMinScale = std::min(mWidthScale, mHeightScale);
        mMaxScale = std::max(mWidthScale, mHeightScale);
        
        Peach3DInfoLog("Set design size:(%.1fx%.1f), width:%.2f, height:%.2f",
                       mDesignScreenSize.x, mDesignScreenSize.y, mWidthScale, mHeightScale);
    }
    
    void LayoutManager::setScreenSize(const Vector2& size)
    {
        mIsLandscape = (size.x > size.y);
        float oldWidth = mScreenSize.x, oldHeight = mScreenSize.y;
        mScreenSize = size;
        // reset design size and scales
        if (oldWidth < FLT_EPSILON || oldHeight < FLT_EPSILON) {
            mDesignScreenSize = mScreenSize;
            mWidthScale = mHeightScale = mMinScale = mMaxScale = 1.0f;
        }
    }
}