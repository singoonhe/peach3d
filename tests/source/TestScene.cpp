//
//  TestScene.cpp
//  test
//
//  Created by singoon.he on 9/22/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#include "TestScene.h"
#include "UIScene.h"
#include "ActionScene.h"
#include "MeshScene.h"
#include "TouchScene.h"

struct SceneLinker {
    std::string title;
    std::function<IScene*()> callback;
};

std::vector<SceneLinker> gTestSceneList = {{"UI Test", []()->BaseScene* {return new UIScene();}},
    {"Actions Test", []()->BaseScene* {return new ActionScene();}},
    {"Mesh Test", []()->BaseScene* {return new MeshScene();}},
    {"Touch Test", []()->BaseScene* {return new TouchScene();}}};

bool TestScene::init()
{
    // set clear color grey
    IRender::getSingletonPtr()->setRenderClearColor(Color4(0.5f, 0.5f, 0.5f));
    
    Widget* rootWidget = SceneManager::getSingleton().getRootWidget();
    const Vector2&  screenSize  = LayoutManager::getSingleton().getScreenSize();
    // create function title
    float labelHeight = 35.0f * LayoutManager::getSingleton().getMinScale();
    for (size_t i = 0; i < gTestSceneList.size(); ++i) {
        Label* testLabel = Label::create(gTestSceneList[i].title.c_str(), labelHeight);
        testLabel->setAnchorPoint(Vector2(0.5f, 1.0f));
        testLabel->setPosition(Vector2(screenSize.x / 2.0f, screenSize.y - labelHeight * i));
        testLabel->setClickedAction([i](ClickEvent, const Vector2&){
            IPlatform::getSingleton().replaceWithNewScene(gTestSceneList[i].callback());
        });
        rootWidget->addChild(testLabel);
    }
    
    // register keyboard exit event
    EventDispatcher::getSingletonPtr()->addKeyboardListener(this, std::bind(&TestScene::keyboardEventCallback, this, std::placeholders::_1, std::placeholders::_2));
    return true;
}

void TestScene::keyboardEventCallback(KeyboardEvent event, KeyCode code)
{
    // PC esc or mobile back, show message box
    if ((code == KeyCode::eESC || code == KeyCode::eBack) && event == KeyboardEvent::eKeyDown) {
        if (!mMsgNode) {
            showMessageBox("Are you sure exit ?", [&](ClickEvent, const Vector2&) {
                // exit game
                IPlatform::getSingleton().terminate();
            });
        }
        else {
            // delete msg widget
            deleteMsgWidgetAction(ClickEvent::eClicked, Vector2());
        }
    }
}

void TestScene::showMessageBox(const std::string& text, const ControlListenerFunction& okCallBack)
{
    float minScale = LayoutManager::getSingleton().getMinScale();
    Widget* rootWidget = SceneManager::getSingleton().getRootWidget();
    // create black screen
    const Vector2& nodeSize = Vector2(400.0f * minScale, 200.0f * minScale);
    const Vector2&  screenSize  = LayoutManager::getSingleton().getScreenSize();
    mMsgNode = Widget::create(Vector2(screenSize.x / 2.0f, screenSize.y / 2.0f));
    mMsgNode->setContentSize(nodeSize);
    mMsgNode->setColor(Color4Black);
    mMsgNode->setAnchorPoint(Vector2(0.5, 0.5));
    mMsgNode->setAlpha(0.5f);
    rootWidget->addChild(mMsgNode);
    // create notice label
    Label* noticeLabel = Label::create(text, 30.0f * minScale);
    noticeLabel->setPosition(Vector2(nodeSize.x * 0.5f, nodeSize.y * 0.75f));
    mMsgNode->addChild(noticeLabel);
    
    // create ok button
    Button* okButton = Button::create("press_normal.png", "press_down.png", "press_highlight.png");
    okButton->setPosition(Vector2(nodeSize.x * 0.25f, nodeSize.y * 0.25f));
    okButton->setTitleText("Ok");
    okButton->setClickedAction(okCallBack);
    mMsgNode->addChild(okButton);
    // create cancel button
    Button* cancelButton = Button::create("press_normal.png", "press_down.png", "press_highlight.png");
    cancelButton->setPosition(Vector2(nodeSize.x * 0.75f, nodeSize.y * 0.25f));
    cancelButton->setTitleText("Cancel");
    cancelButton->setClickedAction(std::bind(&TestScene::deleteMsgWidgetAction, this, std::placeholders::_1, std::placeholders::_2));
    mMsgNode->addChild(cancelButton);
}

void TestScene::deleteMsgWidgetAction(ClickEvent, const Vector2&)
{
    if (mMsgNode) {
        mMsgNode->deleteFromParent();
        mMsgNode = nullptr;
    }
}

void TestScene::update(float lastFrameTime)
{
}