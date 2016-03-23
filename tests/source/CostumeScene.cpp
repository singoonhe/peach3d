//
//  CostumeScene.cpp
//  test
//
//  Created by singoon.he on 3/22/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "CostumeScene.h"

bool CostumeScene::init()
{
    // init sample list
    mSampleList.push_back([]()->BaseSample* {return new CostumeSample();});
    
    // init base scene
    BaseScene::init();
    return true;
}

void CostumeSample::init(Widget* parentWidget)
{
    // only set title here
    mTitle = "Constume Change";
    
    auto girlMesh = ResourceManager::getSingleton().addMesh("girl.pmt");
    auto texNode = SceneManager::getSingleton().getRootSceneNode()->createChild(Vector3(0.f, -5.f, 0.f));
    texNode->attachMesh(girlMesh);
    // rotate repeat
    texNode->runAction(Repeat::createForever(RotateBy3D::create(Vector3(0.0f, DEGREE_TO_RADIANS(360.0f), 0.0f), 5.0f)));
}