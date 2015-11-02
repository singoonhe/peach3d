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

struct SceneLinker {
    std::string title;
    std::function<IScene*()> callback;
};

std::vector<SceneLinker> gTestSceneList = {{"UI Test", []()->BaseScene* {return new UIScene();}},
    {"Actions Test", []()->BaseScene* {return new ActionScene();}}};

bool TestScene::init()
{
    // set clear color grey
    IRender::getSingletonPtr()->setRenderClearColor(Color4(0.5f, 0.5f, 0.5f));
    
    Widget* rootWidget = SceneManager::getSingleton().getRootWidget();
    const Vector2&  screenSize  = LayoutManager::getSingleton().getScreenSize();
    // create function title
    float labelHeight = 35.0f * LayoutManager::getSingleton().getMinScale();
    for (auto i = 0; i < gTestSceneList.size(); ++i) {
        Label* testLabel = Label::create(gTestSceneList[i].title.c_str(), labelHeight);
        testLabel->setAnchorPoint(Vector2(0.5f, 1.0f));
        testLabel->setPosition(Vector2(screenSize.x / 2.0f, screenSize.y - labelHeight * i));
        testLabel->setClickedAction([i](ClickEvent, const Vector2&){
            IPlatform::getSingleton().replaceWithNewScene(gTestSceneList[i].callback());
        });
        rootWidget->addChild(testLabel);
    }
    
    return true;
}

void TestScene::update(float lastFrameTime)
{
}