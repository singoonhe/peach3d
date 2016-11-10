//
//  DollScene.cpp
//  test
//
//  Created by singoon.he on 3/22/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "DollScene.h"
#include "C3tLoader.h"

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
    mDesc = "Change girl head and weapons";
    
    const char* headName = "head";
    // load girl mesh
    ResourceManager::getSingleton().registerResourceLoaderFunction("c3t", C3tLoader::c3tMeshDataParse);
    auto girlMesh = ResourceManager::getSingleton().addMesh("girl.c3t");
    auto girl1Mesh = ResourceManager::getSingleton().addMesh("girl_head.c3t");
    // save head object
    mHeadObj = girlMesh->getObjectByName(headName);
    mHead1Obj = girl1Mesh->getObjectByName(headName);
    
    // create girl node
    mGirlNode = SceneManager::getSingleton().getRootSceneNode()->createChild(Vector3(0.f, -6.f, -10.f));
    mGirlNode->attachMesh(girlMesh);
    mGirlNode->setScale(Vector3(0.01f));
//    mGirlNode->setRotation(Vector3(DEGREE_TO_RADIANS(90.f), 0.f, DEGREE_TO_RADIANS(180.f)));
    mGirlNode->runAnimate("Take 001");
    // rotate repeat
    mGirlNode->runAction(Repeat::createForever(RotateBy3D::create(Vector3(0.0f, DEGREE_TO_RADIANS(360.0f), 0.0f), 5.0f)));
    
    const Vector2&  screenSize  = LayoutManager::getSingleton().getScreenSize();
    // create change head button
    Button* headButton = Button::create("press_normal.png");
    headButton->setPosition(Vector2(100.f, screenSize.y / 2.0f + 50));
    headButton->setTitleText("Head2");
    headButton->setClickedAction([&, headButton, headName](const Vector2&){
        auto curText = headButton->getTitleText();
        auto headNode = mGirlNode->getRenderNode(headName);
        if (strcmp(curText, "Head1") == 0) {
            headButton->setTitleText("Head2");
            headNode->resetRenderObject(mHeadObj);
        }
        else {
            headButton->setTitleText("Head1");
            headNode->resetRenderObject(mHead1Obj);
        }
    });
    parentWidget->addChild(headButton);
    // create equip button
    Button* weaponButton = Button::create("press_normal.png");
    weaponButton->setPosition(Vector2(100.f, screenSize.y / 2.0f - 50));
    weaponButton->setTitleText("equip");
    weaponButton->setClickedAction([&, weaponButton](const Vector2&){
        auto curText = weaponButton->getTitleText();
        if (strcmp(curText, "unequip") == 0) {
            weaponButton->setTitleText("equip");
            if (mWeaponNode) {
                mGirlNode->detachNode(mWeaponNode);
                mWeaponNode = nullptr;
            }
        }
        else {
            weaponButton->setTitleText("unequip");
            auto weaponMesh = ResourceManager::getSingleton().addMesh("girl_weapon.pmt");
            mWeaponNode = mGirlNode->createChild();
            mWeaponNode->attachMesh(weaponMesh);
            mGirlNode->attachNodeToBone("Bip001 L Hand", mWeaponNode);
        }
    });
    parentWidget->addChild(weaponButton);
}
