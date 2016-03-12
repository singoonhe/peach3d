//
//  LightScene.cpp
//  test
//
//  Created by singoon.he on 3/8/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "LightScene.h"

bool LightScene::init()
{
    // init sample list
    mSampleList.push_back([]()->BaseSample* {return new LightSample();});
    
    // init base scene
    BaseScene::init();
    return true;
}

void LightSample::init(Widget* parentWidget)
{
    const float fixWidth = 40.f;
    // set ortho projective
    auto screenSize = LayoutManager::getSingleton().getScreenSize();
    float calcHeight = screenSize.y * fixWidth / screenSize.x;
    SceneManager::getSingleton().setOrthoProjection(- fixWidth / 2.f, fixWidth / 2.f, - calcHeight / 2.f, calcHeight / 2.f, -10, 10);
    // add a direction light
    Light dirL(Vector3(0.f, -1.f, -1.f), Color3White);
    SceneManager::getSingleton().addNewLight(dirL);
    // only set title here
    mTitle = "Direction Light";
    mDesc = "pmt using vertex normal, obj using face normal";
    
    // create some spheres
    auto sphereMesh = ResourceManager::getSingleton().addMesh("sphere.pmt");
    auto sphereNode = SceneManager::getSingleton().getRootSceneNode()->createChild(Vector3(0.f, 0.f, 0.f));
    sphereNode->attachMesh(sphereMesh);
    
    // create some spheres from obj
    auto objMesh = ResourceManager::getSingleton().addMesh("sphere.obj");
    auto objNode = SceneManager::getSingleton().getRootSceneNode()->createChild(Vector3(6.f, 0.f, 0.f));
    objNode->attachMesh(objMesh);
}

LightSample::~LightSample()
{
    // delete all lights
    SceneManager::getSingleton().deleteAllLights();
    // reset projective
    auto screenSize = LayoutManager::getSingleton().getScreenSize();
    SceneManager::getSingleton().setPerspectiveProjection(90.f, screenSize.x/screenSize.y);
}