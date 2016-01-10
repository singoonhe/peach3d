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
    mSampleList.push_back([]()->BaseSample* {return new ObjTexSample();});
    mSampleList.push_back([]()->BaseSample* {return new DrawModeSample();});
    
    // init base scene
    BaseScene::init();
    return true;
}

void ObjSample::init(Widget* parentWidget)
{
    // set title and desc
    mTitle = "Obj format file Sample";
    mDesc = "load .obj format file without texture, auto draw batch";
    // load mesh
    auto cubeMesh = ResourceManager::getSingleton().addMesh("Cube.obj");
    auto cubeNode = SceneManager::getSingleton().getRootSceneNode()->createChild();
    cubeNode->attachMesh(cubeMesh);
    // rotate repeat
    cubeNode->runAction(Repeat::createForever(RotateBy3D::create(Vector3(0.0f, DEGREE_TO_RADIANS(360.0f), 0.0f), 5.0f)));
    
    auto cubeNode1 = SceneManager::getSingleton().getRootSceneNode()->createChild(Vector3(-6.f, 0.f, 0.f));
    cubeNode1->attachMesh(cubeMesh);
    // modify material color
    Material node1Mat = cubeNode1->getRenderNode("Cube")->getMaterial();
    node1Mat.diffuse = Color4(1.f, 0.f, 0.f, 1.f);
    cubeNode1->getRenderNode("Cube")->setMaterial(node1Mat);
    // rotate repeat
    cubeNode1->runAction(Repeat::createForever(RotateBy3D::create(Vector3(0.0f, 0.0f, DEGREE_TO_RADIANS(360.0f)), 5.0f)));
    
    auto cubeNode2 = SceneManager::getSingleton().getRootSceneNode()->createChild(Vector3(6.f, 0.f, 0.f));
    cubeNode2->attachMesh(cubeMesh);
    // scale node can't affect others
    cubeNode2->setScale(Vector3(2.f, 2.f, 2.f));
    // rotate repeat
    cubeNode2->runAction(Repeat::createForever(RotateBy3D::create(Vector3(DEGREE_TO_RADIANS(360.0f), 0.0f, 0.0f), 5.0f)));
}

void ObjTexSample::init(Widget* parentWidget)
{
    // set title and desc
    mTitle = "Obj format file with texture Sample";
    mDesc = "load .obj format file with one texture, same texture node will draw batch";
    // load mesh
    auto cubeMesh = ResourceManager::getSingleton().addMesh("texcube.obj");
    auto cubeNode = SceneManager::getSingleton().getRootSceneNode()->createChild();
    cubeNode->attachMesh(cubeMesh);
    // rotate repeat
    cubeNode->runAction(Repeat::createForever(RotateBy3D::create(Vector3(0.0f, DEGREE_TO_RADIANS(360.0f), 0.0f), 5.0f)));
    
    auto cubeNode1 = SceneManager::getSingleton().getRootSceneNode()->createChild(Vector3(-6.f, 0.f, 0.f));
    cubeNode1->attachMesh(cubeMesh);
    // modify material color
    Material node1Mat = cubeNode1->getRenderNode("Cube")->getMaterial();
    node1Mat.diffuse = Color4(1.f, 0.f, 0.f, 1.f);
    cubeNode1->getRenderNode("Cube")->setMaterial(node1Mat);
    cubeNode1->setScale(Vector3(1.5f, 1.5f, 1.5f));
    // rotate repeat
    cubeNode1->runAction(Repeat::createForever(RotateBy3D::create(Vector3(0.0f, 0.0f, DEGREE_TO_RADIANS(360.0f)), 5.0f)));

    auto cubeNode2 = SceneManager::getSingleton().getRootSceneNode()->createChild(Vector3(6.f, 0.f, 0.f));
    cubeNode2->attachMesh(cubeMesh);
    // modify material texture
    cubeNode2->getRenderNode("Cube")->resetTextureByIndex(0, ResourceManager::getSingleton().addTexture("peach3d_leaf.png"));
    // rotate repeat
    cubeNode2->runAction(Repeat::createForever(RotateBy3D::create(Vector3(DEGREE_TO_RADIANS(360.0f), 0.0f, 0.0f), 5.0f)));
}

void DrawModeSample::init(Widget* parentWidget)
{
    // set title and desc
    mTitle = "Draw Mesh with different mode";
    mDesc = "draw .obj file with different mode, same mode node will draw batch";
    // load mesh
    auto rottNode = SceneManager::getSingleton().getRootSceneNode();
    auto cubeMesh = ResourceManager::getSingleton().addMesh("Cube.obj");
    auto cubeNode = rottNode->createChild(Vector3(-6.f, 0.f, 0.f));
    cubeNode->attachMesh(cubeMesh);
    // rotate repeat
    cubeNode->runAction(Repeat::createForever(RotateBy3D::create(Vector3(0.0f, DEGREE_TO_RADIANS(360.0f), 0.0f), 5.0f)));
    
    // create 9 nodes
    for (auto i=0; i<9; i++) {
        auto cubeNodex = rottNode->createChild(Vector3((i % 3) * 4 - 2.f, (i / 3) * 4.f - 4.f, 0.f));
        cubeNodex->setDrawMode(DrawMode::eLine);
        cubeNodex->attachMesh(cubeMesh);
        cubeNodex->runAction(Repeat::createForever(RotateBy3D::create(Vector3(0.0f, DEGREE_TO_RADIANS(180.0f), 0.0f), (i / 3) * 2.f + 1.0f)));
    }
}