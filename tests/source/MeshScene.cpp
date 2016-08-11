//
//  MeshScene.cpp
//  test
//
//  Created by singoon.he on 1/4/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "MeshScene.h"
#include "C3tLoader.h"

bool MeshScene::init()
{
    // init sample list
    mSampleList.push_back([]()->BaseSample* {return new ObjSample();});
    mSampleList.push_back([]()->BaseSample* {return new EngineMeshSample();});
    mSampleList.push_back([]()->BaseSample* {return new DrawModeSample();});
    mSampleList.push_back([]()->BaseSample* {return new OBBSample();});
    
    // init base scene
    BaseScene::init();
    
    // registe *.c3t file loader
    ResourceManager::getSingleton().registerResourceLoaderFunction("c3t", C3tLoader::c3tMeshDataParse);
    return true;
}

void ObjSample::init(Widget* parentWidget)
{
    // set title and desc
    mTitle = "Obj format file Sample";
    mDesc = "load .obj format file , auto draw batch";
    // load mesh
    auto cubeMesh = ResourceManager::getSingleton().addMesh("Cube.obj");
    auto cubeNode = SceneManager::getSingleton().getRootSceneNode()->createChild(Vector3(0.f, 3.f, 0.f));
    cubeNode->attachMesh(cubeMesh);
    // rotate repeat
    cubeNode->runAction(Repeat::createForever(RotateBy3D::create(Vector3(0.0f, DEGREE_TO_RADIANS(360.0f), 0.0f), 5.0f)));
    
    auto cubeNode1 = SceneManager::getSingleton().getRootSceneNode()->createChild(Vector3(-6.f, 3.f, 0.f));
    cubeNode1->attachMesh(cubeMesh);
    // modify material color
    auto cubeObj = cubeNode1->getRenderNode("Cube");
    if (cubeObj) {
        cubeObj->setDiffuse(Color3(1.f, 0.f, 0.f));
    }
    // rotate repeat
    cubeNode1->runAction(Repeat::createForever(RotateBy3D::create(Vector3(0.0f, 0.0f, DEGREE_TO_RADIANS(360.0f)), 5.0f)));
    
    auto cubeNode2 = SceneManager::getSingleton().getRootSceneNode()->createChild(Vector3(6.f, 3.f, 0.f));
    cubeNode2->attachMesh(cubeMesh);
    // scale node can't affect others
    cubeNode2->setScale(Vector3(2.f, 2.f, 2.f));
    // rotate repeat
    cubeNode2->runAction(Repeat::createForever(RotateBy3D::create(Vector3(DEGREE_TO_RADIANS(360.0f), 0.0f, 0.0f), 5.0f)));
    
    /******************************** mesh with texture **************************************/
    // load mesh
    auto texMesh = ResourceManager::getSingleton().addMesh("texcube.obj");
    auto texNode = SceneManager::getSingleton().getRootSceneNode()->createChild(Vector3(0.f, -3.f, 0.f));
    texNode->attachMesh(texMesh);
    // rotate repeat
    texNode->runAction(Repeat::createForever(RotateBy3D::create(Vector3(0.0f, DEGREE_TO_RADIANS(360.0f), 0.0f), 5.0f)));
    
    auto texNode1 = SceneManager::getSingleton().getRootSceneNode()->createChild(Vector3(-6.f, -3.f, 0.f));
    texNode1->attachMesh(texMesh);
    // modify material color
    auto texCubeObj = texNode1->getRenderNode("Cube");
    if (texCubeObj) {
        texCubeObj->setDiffuse(Color3(1.f, 0.f, 0.f));
    }
    texNode1->runAction(Repeat::createListForever(FadeOut::create(0.f, 3.0f), FadeIn::create(1.f, 3.0f), NULL));
    texNode1->setScale(Vector3(1.5f, 1.5f, 1.5f));
    // rotate repeat
    texNode1->runAction(Repeat::createForever(RotateBy3D::create(Vector3(0.0f, 0.0f, DEGREE_TO_RADIANS(360.0f)), 5.0f)));
    
    auto texNode2 = SceneManager::getSingleton().getRootSceneNode()->createChild(Vector3(6.f, -3.f, 0.f));
    texNode2->attachMesh(texMesh);
    // modify material texture
    texNode2->getRenderNode("Cube")->resetTextureByIndex(0, ResourceManager::getSingleton().addTexture("peach3d_leaf.png"));
    // rotate repeat
    texNode2->runAction(Repeat::createForever(RotateBy3D::create(Vector3(DEGREE_TO_RADIANS(360.0f), 0.0f, 0.0f), 5.0f)));
}

void EngineMeshSample::init(Widget* parentWidget)
{
    // set title and desc
    mTitle = "Peach3D Mesh File Sample";
    mDesc = "load *.pmt(Peach3D Mesh Text) and *.pmb(Peach3D Mesh Binary) file";
    auto rootNode = SceneManager::getSingleton().getRootSceneNode();
    auto texMesh = ResourceManager::getSingleton().addMesh("texcube.pmt");
    auto texNode = rootNode->createChild(Vector3(-6.f, 3.f, 0.f));
    texNode->attachMesh(texMesh);
    // rotate repeat
    texNode->runAction(Repeat::createForever(RotateBy3D::create(Vector3(0.0f, 0.0f, DEGREE_TO_RADIANS(360.0f)), 5.0f)));
    
    // create skeleton mesh
    auto skMesh = ResourceManager::getSingleton().addMesh("donghua.pmt");
    auto skNode = rootNode->createChild(Vector3(0.f, 0.f, 0.f));
    skNode->attachMesh(skMesh);
    skNode->runAction(Repeat::createForever(RotateBy3D::create(Vector3(DEGREE_TO_RADIANS(360.0f), 0.0f, 0.0f), 5.0f)));
    skNode->runAnimate("Armature|Take 001|BaseLayer.001");
//    auto skNode1 = rootNode->createChild(Vector3(6.f, 3.f, 0.f));
//    skNode1->attachMesh(skMesh);
//    skNode1->runAnimate("scale");
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

void OBBSample::init(Widget* parentWidget)
{
    // set title and desc
    mTitle = "Draw Mesh with OBB";
    mDesc = "draw .obj file with OBB, all OBB could draw batch";
    // load mesh
    auto rootNode = SceneManager::getSingleton().getRootSceneNode();
    auto cubeMesh = ResourceManager::getSingleton().addMesh("texcube.obj");
    auto cubeNode = rootNode->createChild(Vector3(-6.f, 0.f, 0.f));
    cubeNode->attachMesh(cubeMesh);
    // rotate repeat
    cubeNode->runAction(Repeat::createForever(RotateBy3D::create(Vector3(0.0f, DEGREE_TO_RADIANS(-360.0f), 0.0f), 5.0f)));
    
    // create 9 nodes
    for (auto i=0; i<9; i++) {
        auto cubeNodex = rootNode->createChild(Vector3((i % 3) * 5 - 1.f, (i / 3) * 4.f - 4.f, 0.f));
        cubeNodex->setOBBEnabled(true);
        cubeNodex->attachMesh(cubeMesh);
        cubeNodex->runAction(Repeat::createListForever(MoveBy3D::create(Vector3(0.f, 1.f, 0.f), 0.25f), MoveBy3D::create(Vector3(0.f, -1.f, 0.f), 1.5f), NULL));
    }
}