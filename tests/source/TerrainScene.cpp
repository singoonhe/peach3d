//
//  TerrainScene.cpp
//  test
//
//  Created by singoon.he on 20/02/2017.
//  Copyright © 2017 singoon.he. All rights reserved.
//

#include "TerrainScene.h"

bool TerrainScene::init()
{
    // init sample list
    mSampleList.push_back([]()->BaseSample* {return new TerrainSample();});
    
    // init base scene
    BaseScene::init();
    return true;
}

void TerrainSample::init(Widget* parentWidget)
{
    // only set title here
    mTitle = "work on terrain";
    
    // create terrain
    auto terr = PttLoader::pttTerrainParse("terr_walk.ptt");
    SceneManager::getSingleton().addTerrain(terr);
    
    // transform camera pos
    mActionCamera = SceneManager::getSingleton().getActiveCamera();
    mActionState = mActionCamera->getState();
    //    mActionCamera->unLock();
    mActionCamera->lockToPosition(Vector3(120.f, 0.f, -120.f));
    mActionCamera->setPosition(Vector3(10.f, 10.f, -10.f));
    
    // register touch move event
    static Vector2 lastTouchPos;
    EventDispatcher::getSingletonPtr()->addClickEventListener(parentWidget, [&](ClickEvent event, const Vector2& pos) {
        switch (event) {
            case ClickEvent::eDown:
                lastTouchPos = pos;
                break;
            case ClickEvent::eDrag: {
                auto diff = pos - lastTouchPos;
                mActionCamera->rotate(Vector3(0.f, DEGREE_TO_RADIANS(diff.x), 0.f));
                lastTouchPos = pos;
            }
                break;
            default:
                break;
        }
    });
}