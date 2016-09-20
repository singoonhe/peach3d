//
//  DollScene.cpp
//  test
//
//  Created by singoon.he on 3/22/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "DollScene.h"

bool DollScene::init()
{
    // init sample list
    mSampleList.push_back([]()->BaseSample* {return new DollSample();});
    
    // init base scene
    BaseScene::init();
    return true;
}

void DollSample::init(Widget* parentWidget)
{
    // only set title here
    mTitle = "Doll System";
    mDesc = "Change equips and weapons";
    
    auto girlMesh = ResourceManager::getSingleton().addMesh("girl.pmt");
    auto girlNode = SceneManager::getSingleton().getRootSceneNode()->createChild(Vector3(0.f, -5.f, 0.f));
    girlNode->attachMesh(girlMesh);
    // rotate repeat
    girlNode->runAction(Repeat::createForever(RotateBy3D::create(Vector3(0.0f, DEGREE_TO_RADIANS(360.0f), 0.0f), 5.0f)));
    
    /*
    // attach weapon to right hand
    auto peachMesh = ResourceManager::getSingleton().addMesh("peach.pmt");
    auto peachNode = c3tNode->createChild();
    peachNode->attachMesh(peachMesh);
    peachNode->setOBBEnabled(true);
    //peachNode->runAction(Repeat::createForever(RotateBy3D::create(Vector3(0.0f, DEGREE_TO_RADIANS(360.0f), 0.0f), 5.0f)));
    c3tNode->attachNodeToBone("Bip001 R Hand", peachNode);
    
    auto texMesh = ResourceManager::getSingleton().addMesh("texcube.pmt");
    auto peachNode1 = c3tNode->createChild();
    peachNode1->attachMesh(texMesh);
    peachNode1->setOBBEnabled(true);
    c3tNode->attachNodeToBone("Bip001 L Hand", peachNode1); */
}
