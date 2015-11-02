//
//  ActionScene.cpp
//  test
//
//  Created by singoon.he on 8/5/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#include "ActionScene.h"

bool ActionScene::init()
{
    // init sample list
    mSampleList.push_back([]()->BaseSample* {return new SingleSample();});
    mSampleList.push_back([]()->BaseSample* {return new SequenceSample();});
    mSampleList.push_back([]()->BaseSample* {return new CallFuncSample();});
    
    // init base scene
    BaseScene::init();
    return true;
}

void SingleSample::init(Widget* parentWidget)
{
    // set title and desc
    mTitle = "Single Action Sample";
    mDesc = "some base sprite actions and bezier action";
    // create rotate sprite
    const Vector2&  screenSize  = LayoutManager::getSingleton().getScreenSize();
    Sprite* logo1Sprite = Sprite::create("peach3d_icon.png");
    logo1Sprite->setScale(0.2f);
    logo1Sprite->setPosition(screenSize / 2.0f);
    logo1Sprite->runAction(RotateBy2D::create(DEGREE_TO_RADIANS(360.0f), 2.5f));
    parentWidget->addChild(logo1Sprite);
    // create move sprite
    Sprite* logo2Sprite = Sprite::create("peach3d_icon.png");
    logo2Sprite->setScale(0.2f);
    logo2Sprite->setPosition(Vector2(screenSize.x / 4.0f, screenSize.y / 2.0f));
    logo2Sprite->runAction(MoveBy2D::create(Vector2(screenSize.x / 2.0f, 0.0f), 2.0f));
    parentWidget->addChild(logo2Sprite);
    // create scale sprite
    Sprite* logo3Sprite = Sprite::create("peach3d_icon.png");
    logo3Sprite->setScale(0.1f);
    logo3Sprite->setPosition(Vector2(screenSize.x / 4.0f, screenSize.y / 2.0f));
    logo3Sprite->runAction(ScaleTo2D::create(Vector2(0.2f, 0.2f), 1.5f));
    parentWidget->addChild(logo3Sprite);
    
    // create bezier action
    Sprite* logo4Sprite = Sprite::create("peach3d_icon.png");
    logo4Sprite->setScale(0.1f);
    logo4Sprite->setPosition(Vector2(screenSize.x / 4.0f, screenSize.y / 3.0f));
    const std::vector<Vector2> ctrlPoss = {Vector2(screenSize.x / 4.0f, screenSize.y), Vector2(screenSize.x * 3.0f / 4.0f, screenSize.y)};
    logo4Sprite->runAction(Bezier2D::create(Vector2(screenSize.x * 3.0f / 4.0f, screenSize.y / 3.0f), ctrlPoss, 2.5f));
    parentWidget->addChild(logo4Sprite);
}

void SequenceSample::init(Widget* parentWidget)
{
    // set title and desc
    mTitle = "Sequence Action Sample";
    mDesc = "repeat/spawn/sequnce actions";
    // create repeat forever sprite
    const Vector2&  screenSize  = LayoutManager::getSingleton().getScreenSize();
    Sprite* logo1Sprite = Sprite::create("peach3d_icon.png");
    logo1Sprite->setScale(0.2f);
    logo1Sprite->setPosition(Vector2(screenSize.x / 4.0f, screenSize.y / 2.0f));
    logo1Sprite->runAction(Repeat::createForever(RotateBy2D::create(DEGREE_TO_RADIANS(360.0f), 1.5f)));
    parentWidget->addChild(logo1Sprite);
    // create spawn action
    Sprite* logo2Sprite = Sprite::create("peach3d_icon.png");
    logo2Sprite->setScale(0.1f);
    logo2Sprite->setPosition(Vector2(screenSize.x / 2.0f, screenSize.y / 2.0f));
    logo2Sprite->runActionList(Spawn::createList(ScaleTo2D::create(Vector2(0.2f, 0.2f), 1.5f),
                                                 MoveTo2D::create(Vector2(screenSize.x * 3.0f / 4.0f, screenSize.y / 2.0f), 1.5f), NULL),
                               Spawn::createList(ScaleTo2D::create(Vector2(0.1f, 0.1f), 1.0f),
                                                 MoveTo2D::create(Vector2(screenSize.x / 2.0f, screenSize.y / 2.0f), 1.0f), NULL), NULL);
    parentWidget->addChild(logo2Sprite);
    // create scale sprite
    Sprite* logo3Sprite = Sprite::create("peach3d_icon.png");
    logo3Sprite->setScale(0.1f);
    logo3Sprite->setPosition(Vector2(screenSize.x * 3.0f / 4.0f, screenSize.y / 4.0f));
    logo3Sprite->runActionList(MoveTo2D::create(Vector2(screenSize.x * 3.0f / 4.0f, screenSize.y * 3.0f / 4.0f), 1.0f),
                               MoveTo2D::create(Vector2(screenSize.x / 4.0f, screenSize.y * 3.0f / 4.0f), 1.0f),
                               MoveTo2D::create(Vector2(screenSize.x / 4.0f, screenSize.y / 4.0f), 1.0f), NULL);
    parentWidget->addChild(logo3Sprite);
}

void CallFuncSample::init(Widget* parentWidget)
{
    // set title and desc
    mTitle = "CallFunc Action Sample";
    mDesc = "run sequnce actions, then call back function";
    // create repeat forever sprite
    const Vector2&  screenSize  = LayoutManager::getSingleton().getScreenSize();
    Sprite* logo1Sprite = Sprite::create("peach3d_icon.png");
    logo1Sprite->setScale(0.2f);
    logo1Sprite->setPosition(Vector2(screenSize.x / 4.0f, screenSize.y / 2.0f));
    logo1Sprite->runActionList(Repeat::createList(2, Delay::create(0.5f), Visible::create(false),
                                                  Delay::create(0.5f), Visible::create(true), NULL),
                               MoveTo2D::create(Vector2(screenSize.x / 2.0f, screenSize.y / 2.0f), 1.0f),
                               CallFunc::create([=](){
        Label* noticeLabel = Label::create("Logo arrived center position!", 30 * LayoutManager::getSingleton().getMinScale());
        noticeLabel->setPosition(Vector2(screenSize.x / 2.0f, screenSize.y / 4.0f));
        noticeLabel->setColor(Color4Yellow);
        parentWidget->addChild(noticeLabel);
    }), NULL);
    parentWidget->addChild(logo1Sprite);
}