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
    float halfPace = terr->getLandPace() * 0.5f;
    mActionCamera->lockToPosition(Vector3(terr->getWidthCount() * halfPace, 0.f, -terr->getHeightCount() * halfPace));
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
                mActionCamera->rotate(Vector3(DEGREE_TO_RADIANS(diff.y * 0.05f), -DEGREE_TO_RADIANS(diff.x * 0.5f), 0.f));
                lastTouchPos = pos;
            }
                break;
            default:
                break;
        }
    });
}
