//
//  MeshScene.cpp
//  test
//
//  Created by singoon.he on 1/4/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "MeshScene.h"

bool MeshScene::init()
{
    // init sample list
    mSampleList.push_back([]()->BaseSample* {return new ObjSample();});
    
    // init base scene
    BaseScene::init();
    return true;
}

void ObjSample::init(Widget* parentWidget)
{
    // set title and desc
    mTitle = "Obj format file Sample";
    mDesc = "load .obj format file without texture";
    // load mesh
    auto cubeMesh = ResourceManager::getSingleton().addMesh("Cube.obj");
    auto cubeNode = SceneManager::getSingleton().getRootSceneNode()->createChild();
    cubeNode->attachMesh(cubeMesh);
}