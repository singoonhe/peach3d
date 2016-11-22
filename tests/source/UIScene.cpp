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
    mSampleList.push_back([]()->BaseSample* {return new SpriteSample();});
    mSampleList.push_back([]()->BaseSample* {return new LabelSample();});
    mSampleList.push_back([]()->BaseSample* {return new ButtonSample();});
    mSampleList.push_back([]()->BaseSample* {return new ProgressBarSample();});
    mSampleList.push_back([]()->BaseSample* {return new LayoutSample();});
    
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
    color1Widget->setColor(Color3Red);
    color1Widget->setAnchorPoint(Vector2(1, 0));
    color1Widget->setAlpha(0.5f);
    parentWidget->addChild(color1Widget);
    Widget* color2Widget = Widget::create(Vector2(screenSize.x / 2.0f, screenSize.y / 2.0f));
    color2Widget->setContentSize(Vector2(100, 100));
    color2Widget->setColor(Color3Green);
    color2Widget->setScale(1.5f);
    color2Widget->setAnchorPoint(Vector2(1, 1));
    color2Widget->setAlpha(0.5f);
    parentWidget->addChild(color2Widget);
    Widget* color3Widget = Widget::create(Vector2(screenSize.x / 2.0f, screenSize.y / 2.0f));
    color3Widget->setContentSize(Vector2(200, 200));
    color3Widget->setColor(Color3Blue);
    color3Widget->setAnchorPoint(Vector2(0, 1));
    color3Widget->setAlpha(0.5f);
    parentWidget->addChild(color3Widget);
    Widget* color4Widget = Widget::create(Vector2(screenSize.x / 2.0f, screenSize.y / 2.0f));
    color4Widget->setContentSize(Vector2(100, 100));
    color4Widget->setColor(Color3White);
    color4Widget->setScale(1.5f);
    color4Widget->setAnchorPoint(Vector2(0, 0));
    color4Widget->setAlpha(0.5f);
    parentWidget->addChild(color4Widget);
}

void SpriteSample::init(Widget* parentWidget)
{
    // set title and desc
    mTitle = "Sprite Sample";
    mDesc = "create sprite with file/frame/#xxx";
    
    const Vector2&  screenSize  = LayoutManager::getSingleton().getScreenSize();
    auto firstPosY = screenSize.y * 3.f / 4.f;
    auto firstSpriteY = firstPosY - 80;
    // create sprite with texture file
    Label* label1 = Label::create("texture file name", 20 * LayoutManager::getSingleton().getMinScale());
    label1->setPosition(Vector2(screenSize.x * 0.7f / 4.0f, firstPosY));
    parentWidget->addChild(label1);
    Sprite* sprite1 = Sprite::create("peach3d_1.png");
    sprite1->setPosition(Vector2(screenSize.x * 0.7f / 4.0f, firstSpriteY));
    parentWidget->addChild(sprite1);
    // read frame action from file
    std::vector<TextureFrame> fileLogoList;
    Peach3DAssert(ResourceManager::getSingleton().addTextureFrames("peach3d_log.xml", &fileLogoList), "Load frame file failed");
    // create sprite with texture frame
    Label* label2 = Label::create("texture second frame", 20 * LayoutManager::getSingleton().getMinScale());
    label2->setPosition(Vector2(screenSize.x / 2.f, firstPosY));
    parentWidget->addChild(label2);
    Sprite* sprite2 = Sprite::create(fileLogoList[1]);
    sprite2->setPosition(Vector2(screenSize.x / 2.f, firstSpriteY));
    parentWidget->addChild(sprite2);
    // create sprite with "#xxx" format name
    Label* label3 = Label::create("using \"#peach3d_3.png\"", 20 * LayoutManager::getSingleton().getMinScale());
    label3->setPosition(Vector2(screenSize.x * 3.3f / 4.0f, firstPosY));
    parentWidget->addChild(label3);
    Sprite* sprite3 = Sprite::create("#peach3d_3.png");
    sprite3->setPosition(Vector2(screenSize.x * 3.3f / 4.0f, firstSpriteY));
    parentWidget->addChild(sprite3);
    
    auto secondPosY = screenSize.y * 2 / 5.f;
    auto secondSpriteY = secondPosY - 110;
    // create scale9 type sprite
    Label* label4 = Label::create("scale9 sprite with default center rect", 20 * LayoutManager::getSingleton().getMinScale());
    label4->setPosition(Vector2(screenSize.x / 3.f, secondPosY));
    parentWidget->addChild(label4);
    Sprite* sprite4 = Sprite::create("peach3d_1.png");
    sprite4->setPosition(Vector2(screenSize.x / 3.f, secondSpriteY));
    sprite4->setScale9Enabled(true);
    sprite4->setContentSize(Vector2(150.f));
    parentWidget->addChild(sprite4);
    // create change sprite scale9 type button
    Sprite* sprite5 = Sprite::create("#peach3d_3.png");
    sprite5->setPosition(Vector2(screenSize.x * 2.f / 3.f, secondSpriteY));
    sprite5->setCenterRect(Rect(Vector2(0.2f), Vector2(0.6f)));
    sprite5->setScale9Enabled(true);
    sprite5->setContentSize(Vector2(150.f));
    parentWidget->addChild(sprite5);
    Button* grayButton = Button::create("press_normal.png");
    grayButton->setPosition(Vector2(screenSize.x * 2.f / 3.f, secondPosY));
    grayButton->setTitleText("normal");
    grayButton->setClickedAction([grayButton, sprite5](const Vector2&){
        auto enable = sprite5->isScale9Enabled();
        enable = !enable;
        grayButton->setTitleText(enable ? "normal" : "scale9");
        sprite5->setScale9Enabled(enable);
    });
    parentWidget->addChild(grayButton);
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
    labelBg->setColor(Color3Green);
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
    chineseLabel->setColor(Color3Yellow);
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
    const float firstRowY = screenSize.y * 2.f / 3.0f;
    Button* wholeButton = Button::create("press_normal.png", "press_down.png", "press_highlight.png");
    wholeButton->setPosition(Vector2(screenSize.x / 4.0f, firstRowY));
    parentWidget->addChild(wholeButton);
    // disable button
    Button* grayButton = Button::create("press_normal.png");
    grayButton->setPosition(Vector2(screenSize.x / 2.0f, firstRowY));
    grayButton->setTitleText("change disable");
    grayButton->setClickedAction([grayButton](const Vector2&){
        grayButton->setClickEnabled(false);
        grayButton->setTitleText("disable now");
    });
    parentWidget->addChild(grayButton);
    // title button action
    mTitleButton = Button::create("press_normal.png");
    mTitleButton->setTitleText("show label");
    mTitleButton->setPosition(Vector2(screenSize.x * 3.0f / 4.0f, firstRowY));
    mTitleButton->setClickedAction([&](const Vector2&){
        // create notice label
        if (!mEventLabel) {
            mEventLabel = Label::create("Button click event!!!", 30 * LayoutManager::getSingleton().getMinScale());
            mEventLabel->setPosition(Vector2(screenSize.x * 3.0f / 4.0f, firstRowY + 80.f));
            mEventLabel->setColor(Color3Yellow);
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
    
    // create button from frame
    const float secondRowY = screenSize.y / 3.0f;
    std::vector<TextureFrame> frameList;
    Peach3DAssert(ResourceManager::getSingleton().addTextureFrames("peach3d_ui.xml", &frameList), "Load frame file failed");
    Button* frameButton = Button::create("#common_normal.png", "#common_highlight.png", "#common_down.png", "#common_disable.png");
    frameButton->setPosition(Vector2(screenSize.x / 4.0f, secondRowY));
    frameButton->setTitleText("frame button");
    frameButton->setClickedAction([frameButton](const Vector2&){
        frameButton->setClickEnabled(false);
    });
    parentWidget->addChild(frameButton);
    // button title auto scale
    Button* scaleTitleButton = Button::create("common_normal.png");
    scaleTitleButton->setScale(2.0f);
    scaleTitleButton->setTitleText("scale twice");
    scaleTitleButton->setPosition(Vector2(screenSize.x * 2.f / 3.0f, secondRowY + 60));
    parentWidget->addChild(scaleTitleButton);
    // button title auto scale using scale9 type
    Button* scale9TitleButton = Button::create("common_normal.png");
    scale9TitleButton->setContentSize(scaleTitleButton->getContentSize() * 2.f);
    scale9TitleButton->setTitleText("content twice");
    scale9TitleButton->setScale9Enabled(true);
    scale9TitleButton->setPosition(Vector2(screenSize.x * 2.f / 3.0f, secondRowY - 60));
    scale9TitleButton->setClickedAction([scale9TitleButton](const Vector2&){
        auto enable = scale9TitleButton->isScale9Enabled();
        enable = !enable;
        scale9TitleButton->setScale9Enabled(enable);
        scale9TitleButton->setTitleText(enable ? "content twice" : "scale9 twice");
    });
    parentWidget->addChild(scale9TitleButton);
}

void ProgressBarSample::init(Widget* parentWidget)
{
    // set title and desc
    mTitle = "ProgressBar Sample";
    mDesc = "Layout loading and variables binding, check \"Peach3DLayoutManager.cpp\" for more";
    // create horizontal progress bar
    const Vector2&  screenSize  = LayoutManager::getSingleton().getScreenSize();
    ProgressBar *hbar = ProgressBar::create("bar_hor_bg.png", "bar_hor_bar.png");
    hbar->setPosition(screenSize / 2.f);
    parentWidget->addChild(hbar);
    hbar->setCurrentProgress(0.5f);
    
    // create vertical progress bar
    ProgressBar *vbar = ProgressBar::create("bar_ver_bg.png", "bar_ver_bar.png", ProgressBarType::eVertical);
    vbar->setBarOffset(Vector2Zero);
    vbar->setPosition(Vector2(screenSize.x * 0.1f, screenSize.y * 0.45f));
    parentWidget->addChild(vbar);
    vbar->setCurrentProgress(0.58f);
    vbar->setBarCutMode(ProgressBarCutMode::eLeftBottom);
}

void LayoutSample::init(Widget* parentWidget)
{
    mChangeButton = nullptr;
    mNoticeLabel = nullptr;
    mLogoSprite = nullptr;
    // set title and desc
    mTitle = "Layout Sample";
    mDesc = "Layout loading and variables binding, check \"Peach3DLayoutManager.cpp\" for more";
    // load *.plt(Peach3D Layout Text) file
    LayoutManager::getSingleton().loadLayout("testlayout.plt", parentWidget, [&](const std::string& name, Widget* widget) {
        LAYOUT_BIND_NAME_VARIATE("LogoSprite", mLogoSprite, name, widget)
        LAYOUT_BIND_NAME_VARIATE("NoticeLabel", mNoticeLabel, name, widget)
        LAYOUT_BIND_NAME_VARIATE("ChangeButton", mChangeButton, name, widget)
    }, [&](const std::string& name) {
        if (mChangeButton && mNoticeLabel && mLogoSprite) {
            mChangeButton->setClickedAction([&](const Vector2&){
                bool curGray = !mLogoSprite->isGrayscaleEnabled();
                mLogoSprite->setGrayscaleEnabled(curGray);
                mNoticeLabel->setText(curGray ? "Logo show gray!" : "Logo show normal!");
            });
        }
    });
}
