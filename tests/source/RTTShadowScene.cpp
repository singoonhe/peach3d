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
    mSampleList.push_back([]()->BaseSample* {return new ShadowSample();});
    
    // init base scene
    BaseScene::init();
    return true;
}

void RTTSample::init(Widget* parentWidget)
{
    auto screenSize = LayoutManager::getSingleton().getScreenSize();
    // only set title here
    mTitle = "RTT(render to texture) sample";
    mDesc = "render 3D world to texture, used on sprite";
    
    // create a girl
    auto girlMesh = ResourceManager::getSingleton().addMesh("girl.pmt");
    auto texNode1 = SceneManager::getSingleton().getRootSceneNode()->createChild(Vector3(-6.f, -5.f, 0.f));
    texNode1->attachMesh(girlMesh);
    auto texNode2 = SceneManager::getSingleton().getRootSceneNode()->createChild(Vector3(6.f, -5.f, 0.f));
    texNode2->attachMesh(girlMesh);
    // rotate repeat
    texNode1->runAction(Repeat::createForever(RotateBy3D::create(Vector3(0.0f, DEGREE_TO_RADIANS(360.0f), 0.0f), 5.0f)));
    texNode2->runAction(Repeat::createForever(RotateBy3D::create(Vector3(0.0f, -DEGREE_TO_RADIANS(360.0f), 0.0f), 5.0f)));
    
    // create RTT, move camera to head
    int rw = screenSize.x / 5, rh = screenSize.y / 5;
    TexturePtr rttT = ResourceManager::getSingleton().createRenderTexture(rw, rh);
    rttT->setBeforeRenderingFunc([]{
        auto activeCamera = SceneManager::getSingleton().getActiveCamera();
        activeCamera->setPosition(Vector3(-6.f, 3.5f, 3.f));
        activeCamera->lockToPosition(Vector3(-6.f, 3.5f, 0.f));
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
    
    // create show/hide button, control sprite visible
    Button* ncButton = Button::create("press_normal.png");
    ncButton->setPosition(Vector2(screenSize.x, screenSize.y / 2.f + rh / 2.f));
    ncButton->setAnchorPoint(Vector2(1.f, 0.f));
    ncButton->setTitleText("Hide");
    ncButton->setClickedAction([&, ncButton, rttSprite](const Vector2&){
        auto visible = rttSprite->getVisible();
        visible = !visible;
        ncButton->setTitleText(visible ? "Hide" : "Show");
        rttSprite->setVisible(visible);
    });
    parentWidget->addChild(ncButton);
    
    // create RTT2
    TexturePtr rttT2 = ResourceManager::getSingleton().createRenderTexture(rw, rh);
    rttT2->setBeforeRenderingFunc([texNode1]{
        texNode1->setScale(Vector3(1.5f));
        IRender::getSingleton().setRenderClearColor(Color4(0.f, 0.f, 1.f));
    });
    rttT2->setAfterRenderingFunc([texNode1]{
        texNode1->setScale(Vector3(1.0f));
        IRender::getSingleton().setRenderClearColor(Color4Gray);
    });
    auto rttSprite2 = Sprite::create(TextureFrame(rttT2));
    rttSprite2->setPosition(Vector2(rw / 2.f, screenSize.y / 2.f));
    parentWidget->addChild(rttSprite2);
}

void ShadowSample::init(Widget* parentWidget)
{
    // only set title here
    mTitle = "shadow sample";
    mDesc = "render 3D world to texture, used on sprite";
    
    // add a dot light
    Vector3 initPos(5.f, 5.f, 0.f);
    mDotLight = SceneManager::getSingleton().addNewLight();
    mDotLight->usingAsDot(initPos);
    // add light node
    auto rootNode = SceneManager::getSingleton().getRootSceneNode();
    auto cubeMesh = ResourceManager::getSingleton().addMesh("peach.obj");
    mDotNode = rootNode->createChild(initPos);
    mDotNode->attachMesh(cubeMesh);
    mDotNode->setScale(Vector3(0.2f));
    mDotNode->setLightingEnabled(false);
    // enable shadow
    mDotLight->setShadowEnabled(true);
    
    // create a girl
    auto girlMesh = ResourceManager::getSingleton().addMesh("girl.pmt");
    auto texNode1 = rootNode->createChild(Vector3(0.f, -5.f, 0.f));
    texNode1->attachMesh(girlMesh);
    texNode1->runAction(Repeat::createForever(RotateBy3D::create(Vector3(0.0f, DEGREE_TO_RADIANS(360.0f), 0.0f), 5.0f)));
    // create a plane using cube
    auto cubeMesh1 = ResourceManager::getSingleton().addMesh("texcube.obj");
    auto cubeNode = rootNode->createChild(Vector3(0.f, -15.f, 0.f));
    cubeNode->attachMesh(cubeMesh1);
    cubeNode->setScale(Vector3(10.f));
    // just for test texture
    auto cubeObj = cubeNode->getRenderNode("Cube");
    if (cubeObj) {
        cubeObj->resetTextureByIndex(0, mDotLight->getShadowTexture());
    }
}

ShadowSample::~ShadowSample()
{
    // delete all lights
    SceneManager::getSingleton().deleteAllLights();
}