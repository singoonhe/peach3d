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
            "DisableTexture", "NormalCoord", "DownCoord", "HighlightCoord", "DisableCoord", "Anchor", "BindPosition", "AutoPosition",
            "Position", "AutoScale", "Scale", "Text", "FontSize", "AutoFontSize", "HAlignment", "VAlignment", "TitleOffset",
            "SwallowEvents", "FillColor", "Alpha", "Color", "Texture", "Coord", "Visible" };
        for (auto i = 0; i < static_cast<int>(LayoutAttrType::eCount); ++i) {
            mLoadAttrMap.insert(std::pair<std::string, LayoutAttrType>(attrStrList[i], static_cast<LayoutAttrType>(i)));
        }
    }

    bool LayoutManager::loadLayout(const std::string& file, Widget* parent, const LayoutVarBindFunction& bindFunc, const LayoutLoadOverFunction& overFunc)
    {
        bool loadRes = false;
        ulong fileLength = 0;
        uchar* fileData = ResourceManager::getSingleton().getFileData(file.c_str(), &fileLength);
        if (fileData && fileLength > 0) {
            loadRes = loadLayout(fileData, fileLength, parent, bindFunc, overFunc);
            free(fileData);
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
                if (!layoutEle) {
                    break;
                }
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
        // define const type and names
        const static std::vector<std::string> bindPosNameV = {"LeftBottom", "LeftCenter", "LeftTop", "CenterBottom", "Center", "CenterTop", "RightBottom", "RightCenter", "RightTop"};
        const static std::vector<std::string> bindScaleNameV = {"Min", "Width", "Height", "Fixed"};
        const float bindScaleV[] = {mMinScale, mWidthScale, mHeightScale, 1.0f};
        static std::map<std::string, TextHAlignment> bindHAlignMap = {std::pair<std::string, TextHAlignment>("Left", TextHAlignment::eLeft), std::pair<std::string, TextHAlignment>("Center", TextHAlignment::eCenter), std::pair<std::string, TextHAlignment>("Right", TextHAlignment::eRight)};
        static std::map<std::string, TextVAlignment> bindVAlignMap = {std::pair<std::string, TextVAlignment>("Top", TextVAlignment::eTop), std::pair<std::string, TextVAlignment>("Center", TextVAlignment::eCenter), std::pair<std::string, TextVAlignment>("Bottom", TextVAlignment::eBottom)};
        // set init value
        Vector2 bindPos, bindPosScale(1.0f, 1.0f), bindScale(1.0f, 1.0f);
        Vector2 readPos, readScale(1.0f, 1.0f);
        float fontSizeScale = 1.0f, readFontSize = 20.0f;
        bool isManualPos = false, isManualScale = false, isManualFontSize = false;
        
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
                        ITexture* attrTex = ResourceManager::getSingleton().addTexture(attrText);
                        dynamic_cast<Button*>(newNode)->setTextureForStatus(attrTex, ButtonStatusNormal);
                    }
                    break;
                case LayoutAttrType::eDownTexture:
                    if (dynamic_cast<Button*>(newNode)) {
                        ITexture* attrTex = ResourceManager::getSingleton().addTexture(attrText);
                        dynamic_cast<Button*>(newNode)->setTextureForStatus(attrTex, ButtonStatusDown);
                    }
                    break;
                case LayoutAttrType::eHighlightTexture:
                    if (dynamic_cast<Button*>(newNode)) {
                        ITexture* attrTex = ResourceManager::getSingleton().addTexture(attrText);
                        dynamic_cast<Button*>(newNode)->setTextureForStatus(attrTex, ButtonStatusHighlight);
                    }
                    break;
                case LayoutAttrType::eDisableTexture:
                    if (dynamic_cast<Button*>(newNode)) {
                        ITexture* attrTex = ResourceManager::getSingleton().addTexture(attrText);
                        dynamic_cast<Button*>(newNode)->setTextureForStatus(attrTex, ButtonStatusDisable);
                    }
                    break;
                case LayoutAttrType::eNormalCoord:
                    if (dynamic_cast<Button*>(newNode)) {
                        Rect attrRect;
                        sscanf(attrText, "%f,%f,%f,%f", &attrRect.pos.x, &attrRect.pos.y, &attrRect.size.x, &attrRect.size.y);
                        dynamic_cast<Button*>(newNode)->setTextureRectForStatus(attrRect, ButtonStatusNormal);
                    }
                    break;
                case LayoutAttrType::eDownCoord:
                    if (dynamic_cast<Button*>(newNode)) {
                        Rect attrRect;
                        sscanf(attrText, "%f,%f,%f,%f", &attrRect.pos.x, &attrRect.pos.y, &attrRect.size.x, &attrRect.size.y);
                        dynamic_cast<Button*>(newNode)->setTextureRectForStatus(attrRect, ButtonStatusDown);
                    }
                    break;
                case LayoutAttrType::eHighlightCoord:
                    if (dynamic_cast<Button*>(newNode)) {
                        Rect attrRect;
                        sscanf(attrText, "%f,%f,%f,%f", &attrRect.pos.x, &attrRect.pos.y, &attrRect.size.x, &attrRect.size.y);
                        dynamic_cast<Button*>(newNode)->setTextureRectForStatus(attrRect, ButtonStatusHighlight);
                    }
                    break;
                case LayoutAttrType::eDisableCoord:
                    if (dynamic_cast<Button*>(newNode)) {
                        Rect attrRect;
                        sscanf(attrText, "%f,%f,%f,%f", &attrRect.pos.x, &attrRect.pos.y, &attrRect.size.x, &attrRect.size.y);
                        dynamic_cast<Button*>(newNode)->setTextureRectForStatus(attrRect, ButtonStatusDisable);
                    }
                    break;
                case LayoutAttrType::eAnchor: {
                    Vector2 attrAnchor;
                    sscanf(attrText, "%f,%f", &attrAnchor.x, &attrAnchor.y);
                    newNode->setAnchorPoint(attrAnchor);
                }
                    break;
                case LayoutAttrType::eBindPosition:
                    for (size_t i = 0; i < bindPosNameV.size(); ++i) {
                        if (bindPosNameV[i].compare(attrText) == 0) {
                            bindPos = mInnerPoss[i];
                            isManualPos = true;
                            break;
                        }
                    }
                    break;
                case LayoutAttrType::eAutoPosition: {
                    std::vector<std::string> attrList = Utils::split(attrText, ',');
                    for (size_t i = 0; i < attrList.size(); ++i) {
                        std::string termAttr = Utils::trim(attrList[i]);
                        for (size_t j = 0; j < bindScaleNameV.size(); ++j) {
                            if (bindScaleNameV[j] == termAttr) {
                                if (i == 0) {
                                    bindPosScale.x = bindScaleV[j];
                                }
                                else if (i == 1) {
                                    bindPosScale.y = bindScaleV[j];
                                }
                                isManualPos = true;
                                break;
                            }
                        }
                    }
                }
                    break;
                case LayoutAttrType::ePosition: {
                    sscanf(attrText, "%f,%f", &readPos.x, &readPos.y);
                    isManualPos = true;
                }
                    break;
                case LayoutAttrType::eAutoScale: {
                    std::vector<std::string> attrList = Utils::split(attrText, ',');
                    for (size_t i = 0; i < attrList.size(); ++i) {
                        std::string termAttr = Utils::trim(attrList[i]);
                        for (size_t j = 0; j < bindScaleNameV.size(); ++j) {
                            if (bindScaleNameV[j] == termAttr) {
                                if (i == 0) {
                                    bindScale.x = bindScaleV[j];
                                }
                                else if (i == 1) {
                                    bindScale.y = bindScaleV[j];
                                }
                                isManualScale = true;
                                break;
                            }
                        }
                    }
                }
                    break;
                case LayoutAttrType::eScale: {
                    sscanf(attrText, "%f,%f", &readScale.x, &readScale.y);
                    isManualScale = true;
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
                    sscanf(attrText, "%f", &readFontSize);
                    isManualFontSize = true;
                }
                    break;
                case LayoutAttrType::eAutoFontSize:
                    for (size_t j = 0; j < bindScaleNameV.size(); ++j) {
                        if (bindScaleNameV[j].compare(attrText) == 0) {
                            fontSizeScale = bindScaleV[j];
                            isManualFontSize = true;
                            break;
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
                    Color4 fillColor;
                    sscanf(attrText, "%f,%f,%f,%f", &fillColor.r, &fillColor.g, &fillColor.b, &fillColor.a);
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
                    Color4 attrColor;
                    sscanf(attrText, "%f,%f,%f,%f", &attrColor.r, &attrColor.g, &attrColor.b, &attrColor.a);
                    newNode->setColor(attrColor);
                }
                    break;
                case LayoutAttrType::eTexture:
                    if (dynamic_cast<Sprite*>(newNode)) {
                        ITexture* attrTex = ResourceManager::getSingleton().addTexture(attrText);
                        dynamic_cast<Sprite*>(newNode)->setTexture(attrTex);
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
        
        // set position and scale
        if (isManualPos) {
            newNode->setPosition(bindPos + readPos * bindPosScale);
        }
        if (isManualScale) {
            newNode->setScale(readScale * bindScale);
        }
        if (isManualFontSize && dynamic_cast<Label*>(newNode)) {
            dynamic_cast<Label*>(newNode)->setFontSize(fontSizeScale * readFontSize);
        }
        else if (isManualFontSize && dynamic_cast<Button*>(newNode)) {
            dynamic_cast<Button*>(newNode)->setTitleFontSize(fontSizeScale * readFontSize);
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
            mWidthScale = mHeightScale = mMinScale = 1.0f;
        }
        
        // format inner positions
        mInnerPoss[1] = Vector2(0.0f, mScreenSize.y / 2.0f);
        mInnerPoss[2] = Vector2(0.0f, mScreenSize.y);
        mInnerPoss[3] = Vector2(mScreenSize.x / 2.0f, 0.0f);
        mInnerPoss[4] = Vector2(mScreenSize.x / 2.0f, mScreenSize.y / 2.0f);
        mInnerPoss[5] = Vector2(mScreenSize.x / 2.0f, mScreenSize.y);
        mInnerPoss[6] = Vector2(mScreenSize.x, 0.0f);
        mInnerPoss[7] = Vector2(mScreenSize.x, mScreenSize.y / 2.0f);
        mInnerPoss[8] = Vector2(mScreenSize.x, mScreenSize.y);
    }
}