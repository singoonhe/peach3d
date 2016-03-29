//
//  RTTShadowScene.cpp
//  test
//
//  Created by singoon.he on 3/29/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "RTTShadowScene.h"

bool RTTShadowScene::init()
{
    // init sample list
    mSampleList.push_back([]()->BaseSample* {return new RTTSample();});
    
    // init base scene
    BaseScene::init();
    return true;
}

void RTTSample::init(Widget* parentWidget)
{
    auto screenSize = LayoutManager::getSingleton().getScreenSize();
    // only set title here
    mTitle = "Direction Light";
    mDesc = "*.pmt using vertex normal, *.obj using face normal";
    
    // create a girl
    auto girlMesh = ResourceManager::getSingleton().addMesh("girl.pmt");
    auto texNode = SceneManager::getSingleton().getRootSceneNode()->createChild(Vector3(0.f, -5.f, 0.f));
    texNode->attachMesh(girlMesh);
    // rotate repeat
    texNode->runAction(Repeat::createForever(RotateBy3D::create(Vector3(0.0f, DEGREE_TO_RADIANS(360.0f), 0.0f), 5.0f)));
    
    // create RTT
    int rw = screenSize.x / 10, rh = screenSize.y / 10;
    ITexture* rttT = ResourceManager::getSingleton().createRenderTexture(rw, rh);
    rttT->setBeforeRenderingFunc([]{
        auto activeCamera = SceneManager::getSingleton().getActiveCamera();
        activeCamera->setPosition(Vector3(0.f, 3.5f, 3.f));
        activeCamera->lockToPosition(Vector3(0.f, 3.5f, 0.f));
        IRender::getSingleton().setRenderClearColor(Color4(1.f, 0.f, 0.f));
    });
    rttT->setAfterRenderingFunc([]{
        auto activeCamera = SceneManager::getSingleton().getActiveCamera();
        activeCamera->setPosition(Vector3(0.f, 0.f, 10.f));
        activeCamera->lockToPosition(Vector3Zero);
        IRender::getSingleton().setRenderClearColor(Color4Gray);
    });
    // create sprite with RTT
    auto rttSprite = Sprite::create(TextureFrame(rttT));
    rttSprite->setPosition(Vector2(screenSize.x - rw / 2.f, screenSize.y / 2.f));
    parentWidget->addChild(rttSprite);
}