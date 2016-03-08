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
    // add a direction light
    Light dirL(Vector3(1.f, -1.f, 0.f), Color3White);
    SceneManager::getSingleton().addNewLight(dirL);
    // only set title here
    mTitle = "Lighting effect";
    
    // create some balls
}

LightSample::~LightSample()
{
    // delete all lights
    SceneManager::getSingleton().deleteAllLights();
}