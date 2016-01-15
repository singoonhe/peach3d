//
//  TouchScene.cpp
//  test
//
//  Created by singoon.he on 1/15/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "TouchScene.h"

bool TouchScene::init()
{
    // init sample list
    mSampleList.push_back([]()->BaseSample* {return new PickSample();});
    
    // init base scene
    BaseScene::init();
    return true;
}

void PickSample::init(Widget* parentWidget)
{
    mClickedNode = nullptr;
    // set title and desc
    mTitle = "3D pick Sample";
    mDesc = "pick 3D node, ";
    
    // create 9 nodes for pick
    auto rootNode = SceneManager::getSingleton().getRootSceneNode();
    auto cubeMesh = ResourceManager::getSingleton().addMesh("texcube.obj");
    for (auto i=0; i<9; i++) {
        auto cubeNodex = rootNode->createChild(Vector3((i % 3) * 5.f - 5.f, int(i / 3) * 4.f - 4.f, 0.f));
        cubeNodex->attachMesh(cubeMesh);
        cubeNodex->setPickingEnabled(true);
        cubeNodex->runAction(Repeat::createListForever(MoveBy3D::create(Vector3(0.f, 1.f, 0.f), 0.25f), MoveBy3D::create(Vector3(0.f, -1.f, 0.f), 1.5f), nullptr));
    }
    
    // add touch listener
    EventDispatcher::getSingletonPtr()->addClickEventListener(parentWidget, [&](ClickEvent event, const std::vector<Vector2>& poss) {
        switch (event) {
            case ClickEvent::eUp: {
                if (mClickedNode) {
                    mClickedNode->setOBBEnabled(false);
                    mClickedNode = nullptr;
                }
                SceneNode* clNode = SceneManager::getSingleton().getWindowClickedNode(poss[0]);
                if (clNode) {
                    clNode->setOBBEnabled(true);
                    mClickedNode = clNode;
                }
            }
            default:
                break;
        }
    });
}