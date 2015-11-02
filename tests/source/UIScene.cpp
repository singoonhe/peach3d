//
//  UIScene.cpp
//  test
//
//  Created by singoon.he on 8/1/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#include "UIScene.h"

bool UIScene::init()
{
    // init sample list
    mSampleList.push_back([]()->BaseSample* {return new WidgetSample();});
    mSampleList.push_back([]()->BaseSample* {return new LabelSample();});
    mSampleList.push_back([]()->BaseSample* {return new ButtonSample();});
    
    // init base scene
    BaseScene::init();
    return true;
}

void WidgetSample::init(Widget* parentWidget)
{
    // set title and desc
    mTitle = "Widget Sample";
    mDesc = "colorful widgets with anchor point, draw once on GL3 and DX";
    // create four color widget
    const Vector2&  screenSize  = LayoutManager::getSingleton().getScreenSize();
    Widget* color1Widget = Widget::create(Vector2(screenSize.x / 2.0f, screenSize.y / 2.0f));
    color1Widget->setContentSize(Vector2(200, 200));
    color1Widget->setColor(Color4Red);
    color1Widget->setAnchorPoint(Vector2(1, 0));
    color1Widget->setAlpha(0.5f);
    parentWidget->addChild(color1Widget);
    Widget* color2Widget = Widget::create(Vector2(screenSize.x / 2.0f, screenSize.y / 2.0f));
    color2Widget->setContentSize(Vector2(100, 100));
    color2Widget->setColor(Color4Green);
    color2Widget->setScale(1.5f);
    color2Widget->setAnchorPoint(Vector2(1, 1));
    color2Widget->setAlpha(0.5f);
    parentWidget->addChild(color2Widget);
    Widget* color3Widget = Widget::create(Vector2(screenSize.x / 2.0f, screenSize.y / 2.0f));
    color3Widget->setContentSize(Vector2(200, 200));
    color3Widget->setColor(Color4Blue);
    color3Widget->setAnchorPoint(Vector2(0, 1));
    color3Widget->setAlpha(0.5f);
    parentWidget->addChild(color3Widget);
    Widget* color4Widget = Widget::create(Vector2(screenSize.x / 2.0f, screenSize.y / 2.0f));
    color4Widget->setContentSize(Vector2(100, 100));
    color4Widget->setColor(Color4White);
    color4Widget->setScale(1.5f);
    color4Widget->setAnchorPoint(Vector2(0, 0));
    color4Widget->setAlpha(0.5f);
    parentWidget->addChild(color4Widget);
}

void LabelSample::init(Widget* parentWidget)
{
    // set title and desc
    mTitle = "Label Sample";
    mDesc = "multi-color label, alignment, multi-language supported";
    // create multi-color label
    const Vector2&  screenSize  = LayoutManager::getSingleton().getScreenSize();
    Label* colorLabel = Label::create("Format label with {{} {FF0000}sample", 30 * LayoutManager::getSingleton().getMinScale());
    colorLabel->setPosition(Vector2(screenSize.x / 2.0f, screenSize.y * 3.0f / 4.0f));
    parentWidget->addChild(colorLabel);
    // create alignment label
    auto labelDim = Vector2(500, 100);
    Widget* labelBg = Widget::create(Vector2(screenSize.x / 2.0f, screenSize.y / 2.0f));
    labelBg->setContentSize(labelDim);
    labelBg->setColor(Color4Green);
    labelBg->setAlpha(0.5f);
    parentWidget->addChild(labelBg);
    Label* alignLabel = Label::create("Default HAlignment is left, VAlignment is bottom", 20 * LayoutManager::getSingleton().getMinScale());
    alignLabel->setPosition(Vector2(labelDim.x / 2.0f, labelDim.y / 2.0f));
    alignLabel->setContentSize(labelDim);
    labelBg->addChild(alignLabel);
    Label* align2Label = Label::create("Set HAlignment to center, VAlignment to top", 20 * LayoutManager::getSingleton().getMinScale());
    align2Label->setPosition(Vector2(labelDim.x / 2.0f, labelDim.y / 2.0f));
    align2Label->setContentSize(labelDim);
    align2Label->setHAlignment(TextHAlignment::eCenter);
    align2Label->setVAlignment(TextVAlignment::eTop);
    labelBg->addChild(align2Label);
    // create multi-language label
    Label* chineseLabel = Label::create("这里显示黄色中文字符", 30 * LayoutManager::getSingleton().getMinScale());
    chineseLabel->setPosition(Vector2(screenSize.x / 2.0f, screenSize.y / 4.0f));
    chineseLabel->setColor(Color4Yellow);
    parentWidget->addChild(chineseLabel);
}

void ButtonSample::init(Widget* parentWidget)
{
    mParentWidget = parentWidget;
    mEventLabel = nullptr;
    // set title and desc
    mTitle = "Button Sample";
    mDesc = "Whole image button, button auto gray, title button and clicked event";
    // whole button
    const Vector2&  screenSize  = LayoutManager::getSingleton().getScreenSize();
    Button* wholeButton = Button::create("press_normal.png", "press_down.png", "press_highlight.png");
    wholeButton->setPosition(Vector2(screenSize.x / 4.0f, screenSize.y / 2.0f));
    parentWidget->addChild(wholeButton);
    Button* grayButton = Button::create("press_normal.png");
    grayButton->setPosition(Vector2(screenSize.x / 2.0f, screenSize.y / 2.0f));
    grayButton->setClickEnabled(false);
    parentWidget->addChild(grayButton);
    mTitleButton = Button::create("press_normal.png");
    mTitleButton->setTitleText("show label");
    mTitleButton->setPosition(Vector2(screenSize.x * 3.0f / 4.0f, screenSize.y / 2.0f));
    mTitleButton->setClickedAction([&](ClickEvent, const Vector2&){
        // create notice label
        if (!mEventLabel) {
            mEventLabel = Label::create("Button click event!!!", 30 * LayoutManager::getSingleton().getMinScale());
            mEventLabel->setPosition(Vector2(screenSize.x / 2.0f, screenSize.y / 4.0f));
            mEventLabel->setColor(Color4Yellow);
            mParentWidget->addChild(mEventLabel);
            mTitleButton->setTitleText("hide label");
        }
        else {
            mEventLabel->deleteFromParent();
            mEventLabel = nullptr;
            mTitleButton->setTitleText("show label");
        }
    });
    parentWidget->addChild(mTitleButton);
    
    // button title auto scale
    Button* scaleTitleButton = Button::create("press_normal.png");
    scaleTitleButton->setScale(2.0f);
    scaleTitleButton->setTitleText("scale button");
    scaleTitleButton->setPosition(Vector2(screenSize.x * 3.0f / 4.0f, screenSize.y / 4.0f));
    scaleTitleButton->setClickEnabled(false);
    parentWidget->addChild(scaleTitleButton);
}