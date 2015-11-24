//
//  BaseScene.cpp
//  test
//
//  Created by singoon.he on 8/1/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#include "BaseScene.h"
#include "TestScene.h"

bool BaseScene::init()
{
    Widget* rootWidget = SceneManager::getSingleton().getRootWidget();
    const Vector2&  screenSize  = LayoutManager::getSingleton().getScreenSize();
    // create sample root widget
    mSampleWidget = Widget::create(Vector2(screenSize.x / 2.0f, screenSize.y / 2.0f));
    rootWidget->addChild(mSampleWidget);
    // create return button
    Button* exitButton = Button::create("exit_normal.png");
    exitButton->setAnchorPoint(Vector2(0.0f, 1.0f));
    exitButton->setPosition(Vector2(0, screenSize.y));
    exitButton->setClickedAction([](ClickEvent, const Vector2&){
        // return to test scene
        IPlatform::getSingleton().replaceWithNewScene(new TestScene());
    });
    rootWidget->addChild(exitButton);
    
    // create title label
    float minScale = LayoutManager::getSingleton().getMinScale();
    float titleHeight = 30.0f * minScale;
    mTitleLabel = Label::create("Title", 26.0f * minScale, "", TextHAlignment::eCenter);
    mTitleLabel->setAnchorPoint(Vector2(0.5f, 1.0f));
    mTitleLabel->setPosition(Vector2(screenSize.x / 2.0f, screenSize.y));
    rootWidget->addChild(mTitleLabel);
    // create desc label
    mDescLabel = Label::create("Desc", 20.0f * minScale, "", TextHAlignment::eCenter);
    mDescLabel->setAnchorPoint(Vector2(0.5f, 1.0f));
    mDescLabel->setPosition(Vector2(screenSize.x / 2.0f, screenSize.y - titleHeight * 1.5f));
    rootWidget->addChild(mDescLabel);
    
    // create front sample button
    Button* frontButton = Button::create("front_normal.png");
    frontButton->setAnchorPoint(Vector2(0.5f, 0.0f));
    frontButton->setPosition(Vector2(screenSize.x / 2.0f - 100 * minScale, 0.0f));
    frontButton->setClickedAction([&](ClickEvent, const Vector2&){
        int index = (mSampleIndex - 1) < 0 ? (int)mSampleList.size() - 1 : mSampleIndex - 1;
        this->changeToSample(index);
    });
    rootWidget->addChild(frontButton);
    // create restart sample button
    Button* restartButton = Button::create("restart_normal.png");
    restartButton->setAnchorPoint(Vector2(0.5f, 0.0f));
    restartButton->setPosition(Vector2(screenSize.x / 2.0f, 0.0f));
    restartButton->setClickedAction([&](ClickEvent, const Vector2&){
        this->changeToSample(mSampleIndex);
    });
    rootWidget->addChild(restartButton);
    // create next sample button
    Button* nextButton = Button::create("next_normal.png");
    nextButton->setAnchorPoint(Vector2(0.5f, 0.0f));
    nextButton->setPosition(Vector2(screenSize.x / 2.0f + 100 * minScale, 0.0f));
    nextButton->setClickedAction([&](ClickEvent, const Vector2&){
        uint index = (mSampleIndex + 1) >= mSampleList.size() ? 0 : mSampleIndex + 1;
        this->changeToSample(index);
    });
    rootWidget->addChild(nextButton);
    
    // register keyboard back event
    EventDispatcher::getSingletonPtr()->addKeyboardListener(this, [](KeyboardEvent event, KeyCode code){
        if ((code == KeyCode::eESC || code == KeyCode::eBack) && event == KeyboardEvent::eKeyDown) {
            // return to test scene
            IPlatform::getSingleton().replaceWithNewScene(new TestScene());
        }
    });
    
    // display first sample
    if (mSampleList.size() > 0) {
        this->changeToSample(mSampleIndex);
    }
    return true;
}

void BaseScene::changeToSample(int index)
{
    if (mSampleList.size() > 0) {
        if (mCurSample) {
            mSampleWidget->deleteAllChildren();
            delete mCurSample;
        }
        // create new sample and init
        mCurSample = mSampleList[index]();
        mCurSample->init(mSampleWidget);
        mTitleLabel->setText(mCurSample->getSampleTitle());
        bool isDesc = mCurSample->getSampleDesc().size() > 0;
        mDescLabel->setVisible(isDesc);
        if (isDesc) {
            mDescLabel->setText(mCurSample->getSampleDesc());
        }
        mSampleIndex = index;
    }
}

void BaseScene::update(float lastFrameTime)
{
    if (mCurSample) {
        mCurSample->update(lastFrameTime);
    }
}